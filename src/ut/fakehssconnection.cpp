/**
 * @file fakehssconnection.cpp Fake HSS Connection (for testing).
 *
 * Copyright (C) Metaswitch Networks 2017
 * If license terms are provided to you in a COPYING file in the root directory
 * of the source code repository by which you are accessing this code, then
 * the license outlined in that COPYING file applies to your use.
 * Otherwise no rights are granted except for those provided to you by
 * Metaswitch Networks in a separate written agreement.
 */

#include <cstdio>
#include "fakehssconnection.hpp"
#include "gtest/gtest.h"

#include "fakesnmp.hpp"

/// HSSConnection that writes to/reads from a local map rather than the HSS.
/// Optionally accepts a MockHSSConnection object -- if this is provided then
/// (currently only some) methods call through to the corresponding Mock
/// methods so method invocation parameters / counts can be policed by test
/// scripts.  This only enables method invocations to be checked -- it does not
/// allow control of the behaviour of those functions -- in all cases the
/// resulting behaviour is dictated by the FakeHSSConnection class.
FakeHSSConnection::FakeHSSConnection(MockHSSConnection* hss_connection_observer) :
  HSSConnection("localhost",
                NULL,
                NULL,
                &SNMP::FAKE_IP_COUNT_TABLE,
                &SNMP::FAKE_EVENT_ACCUMULATOR_TABLE,
                &SNMP::FAKE_EVENT_ACCUMULATOR_TABLE,
                &SNMP::FAKE_EVENT_ACCUMULATOR_TABLE,
                &SNMP::FAKE_EVENT_ACCUMULATOR_TABLE,
                &SNMP::FAKE_EVENT_ACCUMULATOR_TABLE,
                NULL,
                NULL)
{
  _hss_connection_observer = hss_connection_observer;
}


FakeHSSConnection::~FakeHSSConnection()
{
  flush_all();
}

void FakeHSSConnection::flush_all()
{
  _results.clear();
  _calls.clear();
}

void FakeHSSConnection::set_result(const std::string& url,
                                   const std::string& result)
{
  _results[url] = result;
}

void FakeHSSConnection::set_impu_result(const std::string& impu,
                                        const std::string& type,
                                        const std::string& state,
                                        std::string subxml,
                                        std::string extra_params,
                                        const std::string& wildcard)
{
  std::string url = "/impu/" + Utils::url_escape(impu) + "/reg-data" + extra_params;

  if (subxml.empty())
  {
    subxml = ("<IMSSubscription><ServiceProfile>\n"
              "<PublicIdentity><Identity>"+impu+"</Identity></PublicIdentity>"
              "  <InitialFilterCriteria>\n"
              "  </InitialFilterCriteria>\n"
              "</ServiceProfile></IMSSubscription>");
  }

  std::string chargingaddrsxml = ("<ChargingAddresses>\n"
                                  "  <CCF priority=\"1\">ccf1</CCF>\n"
                                  "  <ECF priority=\"1\">ecf1</ECF>\n"
                                  "  <ECF priority=\"2\">ecf2</ECF>\n"
                                  "</ChargingAddresses>");

  std::string result = ("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
                        "<ClearwaterRegData><RegistrationState>" + state + "</RegistrationState>"
                        + subxml + chargingaddrsxml + "</ClearwaterRegData>");

  _results[url] = result;
}


void FakeHSSConnection::delete_result(const std::string& url)
{
  _results.erase(url);
}

long FakeHSSConnection::put_for_xml_object(const std::string& path, std::string body, bool cache_allowed, rapidxml::xml_document<>*& root, SAS::TrailId trail)
{
  return FakeHSSConnection::get_xml_object(path,
                                           body,
                                           root,
                                           trail);
}


void FakeHSSConnection::set_rc(const std::string& url,
                               long rc)
{
  _rcs[url] = rc;
}


void FakeHSSConnection::delete_rc(const std::string& url)
{
  _rcs.erase(url);
}


long FakeHSSConnection::get_json_object(const std::string& path,
                                        rapidjson::Document*& object,
                                        SAS::TrailId trail)
{
  _calls.insert(UrlBody(path, ""));
  HTTPCode http_code = HTTP_NOT_FOUND;

  std::map<std::string, std::string>::const_iterator i = _results.find(path);

  if (i != _results.end())
  {
    TRC_DEBUG("Found HSS data for %s\n%s", path.c_str(), i->second.c_str());
    object = new rapidjson::Document;
    object->Parse<0>(i->second.c_str());

    if (!object->HasParseError())
    {
      http_code = HTTP_OK;
    }
    else
    {
      // report to the user the failure and their locations in the document.
      TRC_ERROR("Failed to parse Homestead response:\n %s\n %s.\n Error offset: %d",
                path.c_str(),
                i->second.c_str(),
                object->GetErrorOffset());
      delete object;
      object = NULL;
    }
  }
  else
  {
    TRC_DEBUG("Failed to find JSON result for URL %s", path.c_str());
  }

  std::map<std::string, long>::const_iterator i2 = _rcs.find(path);
  if (i2 != _rcs.end())
  {
    http_code = i2->second;
  }

  return http_code;
}

long FakeHSSConnection::get_xml_object(const std::string& path,
                                       rapidxml::xml_document<>*& root,
                                       SAS::TrailId trail)
{
  return get_xml_object(path, "", root, trail);
}

long FakeHSSConnection::get_xml_object(const std::string& path,
                                       std::string body,
                                       rapidxml::xml_document<>*& root,
                                       SAS::TrailId trail)
{
  _calls.insert(UrlBody(path, body));
  HTTPCode http_code = HTTP_NOT_FOUND;

  std::map<std::string, std::string>::const_iterator i = _results.find(path);

  if (i != _results.end())
  {
    root = new rapidxml::xml_document<>;
    try
    {
      root->parse<0>(root->allocate_string(i->second.c_str()));
      http_code = HTTP_OK;
    }
    catch (rapidxml::parse_error& err)
    {
      // report to the user the failure and their locations in the document.
      TRC_ERROR("Failed to parse Homestead response:\n %s\n %s\n %s",
                path.c_str(),
                i->second.c_str(),
                err.what());
      delete root;
      root = NULL;
    }
  }
  else
  {
    TRC_ERROR("Failed to find XML result for URL %s", path.c_str());

    for(std::map<std::string, std::string>::const_iterator it = _results.begin();
        it != _results.end();
        ++it)
    {
      TRC_DEBUG(  "Have: (%s)", it->first.c_str());
    }
  }

  std::map<std::string, long>::const_iterator i2 = _rcs.find(path);
  if (i2 != _rcs.end())
  {
    http_code = i2->second;
  }

  return http_code;
}

bool FakeHSSConnection::url_was_requested(const std::string& url, const std::string& body)
{
  return (_calls.find(UrlBody(url, body)) != _calls.end());
}

HTTPCode FakeHSSConnection::update_registration_state(const std::string& public_user_identity,
                                                      const std::string& private_user_identity,
                                                      const std::string& type,
                                                      std::string server_name,
                                                      SAS::TrailId trail)
{
  if (_hss_connection_observer != NULL)
  {
    _hss_connection_observer->update_registration_state(public_user_identity,
                                                        private_user_identity,
                                                        type,
                                                        server_name,
                                                        trail);
  }

  return HSSConnection::update_registration_state(public_user_identity,
                                                  private_user_identity,
                                                  type,
                                                  server_name,
                                                  trail);
}

HTTPCode FakeHSSConnection::update_registration_state(const std::string& public_user_identity,
                                                      const std::string& private_user_identity,
                                                      const std::string& type,
                                                      std::string& regstate,
                                                      std::string server_name,
                                                      std::map<std::string, Ifcs >& ifcs_map,
                                                      AssociatedURIs& associated_uris,
                                                      std::deque<std::string>& ccfs,
                                                      std::deque<std::string>& ecfs,
                                                      SAS::TrailId trail)
{
  if (_hss_connection_observer != NULL)
  {
    _hss_connection_observer->update_registration_state(public_user_identity,
                                                        private_user_identity,
                                                        type,
                                                        regstate,
                                                        server_name,
                                                        ifcs_map,
                                                        associated_uris,
                                                        ccfs,
                                                        ecfs,
                                                        trail);
  }

  return HSSConnection::update_registration_state(public_user_identity,
                                                  private_user_identity,
                                                  type,
                                                  regstate,
                                                  server_name,
                                                  ifcs_map,
                                                  associated_uris,
                                                  ccfs,
                                                  ecfs,
                                                  trail);
}

