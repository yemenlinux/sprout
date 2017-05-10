/**
 * @file aschain_test.cpp UT for Sprout AsChain module
 *
 * Project Clearwater - IMS in the Cloud
 * Copyright (C) 2013  Metaswitch Networks Ltd
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or (at your
 * option) any later version, along with the "Special Exception" for use of
 * the program along with SSL, set forth below. This program is distributed
 * in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 * without even the implied warranty of MERCHANTABILITY or FITNESS FOR
 * A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details. You should have received a copy of the GNU General Public
 * License along with this program.  If not, see
 * <http://www.gnu.org/licenses/>.
 *
 * The author can be reached by email at clearwater@metaswitch.com or by
 * post at Metaswitch Networks Ltd, 100 Church St, Enfield EN2 6BQ, UK
 *
 * Special Exception
 * Metaswitch Networks Ltd  grants you permission to copy, modify,
 * propagate, and distribute a work formed by combining OpenSSL with The
 * Software, or a work derivative of such a combination, even if such
 * copying, modification, propagation, or distribution would otherwise
 * violate the terms of the GPL. You must comply with the GPL in all
 * respects for all of the code used other than OpenSSL.
 * "OpenSSL" means OpenSSL toolkit software distributed by the OpenSSL
 * Project and licensed under the OpenSSL Licenses, or a work based on such
 * software and licensed under the OpenSSL Licenses.
 * "OpenSSL Licenses" means the OpenSSL License and Original SSLeay License
 * under which the OpenSSL Project distributes the OpenSSL toolkit software,
 * as those licenses appear in the file LICENSE-OPENSSL.
 */

#include <string>
#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include "utils.h"
#include "siptest.hpp"
#include "pjutils.h"
#include "stack.h"

#include "aschain.h"
#include "mmtel.h"

using namespace std;
using testing::MatchesRegex;

/// Fixture for AsChainTest
class AsChainTest : public SipTest
{
public:
  AsChainTable* _as_chain_table;

  static void SetUpTestCase()
  {
    SipTest::SetUpTestCase();
  }

  static void TearDownTestCase()
  {
    SipTest::TearDownTestCase();
  }

  AsChainTest() : SipTest(NULL)
  {
    _as_chain_table = new AsChainTable();
  }

  ~AsChainTest()
  {
    delete _as_chain_table; _as_chain_table = NULL;
  }

  void create_invite(pjsip_tx_data** tdata)
  {
    string str("INVITE sip:5755550099@homedomain SIP/2.0\n"
               "Via: SIP/2.0/TCP 10.64.90.97:50693;rport;branch=z9hG4bKPjPtKqxhkZnvVKI2LUEWoZVFjFaqo.cOzf;alias\n"
               "Max-Forwards: 69\n"
               "From: <sip:5755550018@homedomain>;tag=13919SIPpTag0011234\n"
               "To: <sip:5755550099@homedomain>\n"
               "Contact: <sip:5755550018@10.16.62.109:58309;transport=TCP;ob>\n"
               "Call-ID: 1-13919@10.151.20.48\n"
               "CSeq: 4 INVITE\n"
               "Route: <sip:nextnode;transport=TCP;lr;orig>\n"
               "Content-Length: 0\n\n");
    pjsip_rx_data* rdata = build_rxdata(str);
    parse_rxdata(rdata);
    pj_status_t status = PJUtils::create_request_fwd(stack_data.endpt, rdata, NULL, NULL, 0, tdata);
    ASSERT_EQ(PJ_SUCCESS, status);
  }
};

Ifcs matching_ifcs(int count, ...)
{
  std::string xml = R"(<?xml version="1.0" encoding="UTF-8"?><IMSSubscription><ServiceProfile><PublicIdentity><Identity>sip:5755550011@homedomain</Identity></PublicIdentity>)";
  va_list args;
  va_start(args, count);
  for (int i = 0; i < count; i++)
  {
    const char* name = va_arg(args, const char*);
    xml += R"(<InitialFilterCriteria>
                <Priority>1</Priority>
                <ApplicationServer>
                  <ServerName>)";
    xml += name;
    xml +=     R"(</ServerName>
                  <DefaultHandling>0</DefaultHandling>
                </ApplicationServer>
              </InitialFilterCriteria>)";
  }
  va_end(args);

  xml += "</ServiceProfile></IMSSubscription>";

  std::shared_ptr<rapidxml::xml_document<> > ifc_doc (new rapidxml::xml_document<>);
  ifc_doc->parse<0>(ifc_doc->allocate_string(xml.c_str()));
  return Ifcs(ifc_doc, ifc_doc->first_node("IMSSubscription")->first_node("ServiceProfile"), NULL, 0);
}

Ifcs non_matching_ifcs(int count, ...)
{
  std::string xml = R"(<?xml version="1.0" encoding="UTF-8"?><IMSSubscription><ServiceProfile><PublicIdentity><Identity>sip:5755550011@homedomain</Identity></PublicIdentity>)";
  va_list args;
  va_start(args, count);
  for (int i = 0; i < count; i++)
  {
    const char* name = va_arg(args, const char*);
    xml += R"(<InitialFilterCriteria>
                <Priority>1</Priority>
                  <TriggerPoint>
                  <ConditionTypeCNF>0</ConditionTypeCNF>
                  <SPT>
                    <ConditionNegated>0</ConditionNegated>
                    <Group>0</Group>
                    <Method>PUBLISH</Method>
                    <Extension></Extension>
                  </SPT>
                </TriggerPoint>
                <ApplicationServer>
                  <ServerName>)";
    xml += name;
    xml +=     R"(</ServerName>
                  <DefaultHandling>0</DefaultHandling>
                </ApplicationServer>
              </InitialFilterCriteria>)";
  }
  va_end(args);

  xml += "</ServiceProfile></IMSSubscription>";

  std::shared_ptr<rapidxml::xml_document<> > ifc_doc (new rapidxml::xml_document<>);
  ifc_doc->parse<0>(ifc_doc->allocate_string(xml.c_str()));
  return Ifcs(ifc_doc, ifc_doc->first_node("IMSSubscription")->first_node("ServiceProfile"), NULL, 0);
}

TEST_F(AsChainTest, Basics)
{
  IFCConfiguration ifc_configuration(false, false, "");
  Ifcs ifcs1 = matching_ifcs(0);
  AsChain as_chain(_as_chain_table, SessionCase::Originating, "sip:5755550011@homedomain", true, 0, ifcs1, NULL, NULL, ifc_configuration);
  AsChainLink as_chain_link(&as_chain, 0u);

  Ifcs ifcs2 = matching_ifcs(1, "sip:pancommunicon.cw-ngv.com");
  AsChain as_chain2(_as_chain_table, SessionCase::Originating, "sip:5755550011@homedomain", true, 0, ifcs2, NULL, NULL, ifc_configuration);
  AsChainLink as_chain_link2(&as_chain2, 0u);

  Ifcs ifcs3 = matching_ifcs(2, "sip:pancommunicon.cw-ngv.com", "sip:mmtel.homedomain");
  AsChain as_chain3(_as_chain_table, SessionCase::Originating, "sip:5755550011@homedomain", true, 0, ifcs3, NULL, NULL, ifc_configuration);
  AsChainLink as_chain_link3(&as_chain3, 0u);

  EXPECT_THAT(as_chain_link.to_string(), testing::MatchesRegex("AsChain-orig\\[0x[0-9a-f]+\\]:1/0"));
  EXPECT_EQ(SessionCase::Originating, as_chain.session_case());
  EXPECT_EQ("sip:5755550011@homedomain", as_chain._served_user);

  EXPECT_TRUE(as_chain_link.complete()) << as_chain_link.to_string();
  EXPECT_FALSE(as_chain_link2.complete()) << as_chain_link2.to_string();
  EXPECT_FALSE(as_chain_link3.complete()) << as_chain_link3.to_string();

  std::string token = as_chain_link2.next_odi_token();
  AsChainLink res = _as_chain_table->lookup(token);
  EXPECT_EQ(&as_chain2, res._as_chain);
  EXPECT_EQ(1u, res._index);
  EXPECT_TRUE(res.complete());
}

// We have matching standard IFCs - we should select the ASs from
// those IFCs and no more.
TEST_F(AsChainTest, MatchingStandardIFCs)
{
  IFCConfiguration ifc_configuration(false, false, "");
  Ifcs ifcs = matching_ifcs(2, "sip:as1", "sip:as2");
  AsChain as_chain(_as_chain_table, SessionCase::Originating, "sip:5755550011@homedomain", true, 0, ifcs, NULL, NULL, ifc_configuration);
  AsChainLink as_chain_link(&as_chain, 0u);

  pjsip_tx_data* tdata = NULL;
  create_invite(&tdata);
  std::string server_name;

  // Get the first AS
  pjsip_status_code rc = as_chain_link.on_initial_request(tdata->msg, server_name, 1u);
  EXPECT_EQ(server_name, "sip:as1");
  EXPECT_EQ(rc, PJSIP_SC_OK);

  // Get the second AS - successfully pulls the next IFC
  as_chain_link = as_chain_link.next();
  rc = as_chain_link.on_initial_request(tdata->msg, server_name, 1u);
  EXPECT_EQ(server_name, "sip:as2");
  EXPECT_EQ(rc, PJSIP_SC_OK);

  // Get a third AS - this should be empty as we've gone through all the IFCs
  as_chain_link = as_chain_link.next();
  rc = as_chain_link.on_initial_request(tdata->msg, server_name, 1u);
  EXPECT_EQ(server_name, "");
  EXPECT_EQ(rc, PJSIP_SC_OK);
}

// There are no matching standard IFCs, and we're not using default IFCs.
// We shouldn't select any ASs.
TEST_F(AsChainTest, NoMatchingStandardIFCs)
{
  IFCConfiguration ifc_configuration(false, false, "");
  Ifcs ifcs = non_matching_ifcs(2, "sip:as1", "sip:as2");
  AsChain as_chain(_as_chain_table, SessionCase::Originating, "sip:5755550011@homedomain", true, 0, ifcs, NULL, NULL, ifc_configuration);
  AsChainLink as_chain_link(&as_chain, 0u);

  pjsip_tx_data* tdata = NULL;
  create_invite(&tdata);
  std::string server_name;

  pjsip_status_code rc = as_chain_link.on_initial_request(tdata->msg, server_name, 1u);
  EXPECT_EQ(server_name, "");
  EXPECT_EQ(rc, PJSIP_SC_OK);
}

// There are no matching standard IFCs, we're not using default IFCs, and we're
// rejecting when there's no matching IFCs.
// We should select the ASs from the standard IFCs
TEST_F(AsChainTest, MatchingStandardIFCsRejectIfNone)
{
  IFCConfiguration ifc_configuration(false, true, "");
  Ifcs ifcs = matching_ifcs(1, "sip:as1");
  AsChain as_chain(_as_chain_table, SessionCase::Originating, "sip:5755550011@homedomain", true, 0, ifcs, NULL, NULL, ifc_configuration);
  AsChainLink as_chain_link(&as_chain, 0u);

  pjsip_tx_data* tdata = NULL;
  create_invite(&tdata);
  std::string server_name;

  pjsip_status_code rc = as_chain_link.on_initial_request(tdata->msg, server_name, 1u);
  EXPECT_EQ(server_name, "sip:as1");
  EXPECT_EQ(rc, PJSIP_SC_OK);
}

// There are matching standard IFCs, we're not using default IFCs, and we're
// rejecting when there's no matching IFCs.
// We shouldn't select any ASs, and we should have an error response.
TEST_F(AsChainTest, NoMatchingStandardIFCsRejectIfNone)
{
  IFCConfiguration ifc_configuration(false, true, "");
  Ifcs ifcs = non_matching_ifcs(2, "sip:as1", "sip:as2");
  AsChain as_chain(_as_chain_table, SessionCase::Originating, "sip:5755550011@homedomain", true, 0, ifcs, NULL, NULL, ifc_configuration);
  AsChainLink as_chain_link(&as_chain, 0u);

  pjsip_tx_data* tdata = NULL;
  create_invite(&tdata);
  std::string server_name;

  pjsip_status_code rc = as_chain_link.on_initial_request(tdata->msg, server_name, 1u);
  EXPECT_EQ(server_name, "");
  EXPECT_EQ(rc, PJSIP_SC_BAD_REQUEST);
}

// There are matching standard IFCs and matching default IFCs.
// We should select the ASs from the standard IFCs
TEST_F(AsChainTest, MatchingStandardIFCsWithMatchingDefaultIFCs)
{
  IFCConfiguration ifc_configuration(true, true, "");
  Ifcs ifcs = matching_ifcs(2, "sip:as1", "sip:as2");
  Ifcs default_ifcs = matching_ifcs(2, "sip:default_as2", "sip:default_as2");
  AsChain as_chain(_as_chain_table, SessionCase::Originating, "sip:5755550011@homedomain", true, 0, ifcs, NULL, NULL, ifc_configuration);
  as_chain._default_ifcs = default_ifcs.ifcs_list();
  AsChainLink as_chain_link(&as_chain, 0u);

  pjsip_tx_data* tdata = NULL;
  create_invite(&tdata);
  std::string server_name;

  pjsip_status_code rc = as_chain_link.on_initial_request(tdata->msg, server_name, 1u);
  EXPECT_EQ(server_name, "sip:as1");
  EXPECT_EQ(rc, PJSIP_SC_OK);

  as_chain_link = as_chain_link.next();
  rc = as_chain_link.on_initial_request(tdata->msg, server_name, 1u);
  EXPECT_EQ(server_name, "sip:as2");
  EXPECT_EQ(rc, PJSIP_SC_OK);

  as_chain_link = as_chain_link.next();
  rc = as_chain_link.on_initial_request(tdata->msg, server_name, 1u);
  EXPECT_EQ(server_name, "");
  EXPECT_EQ(rc, PJSIP_SC_OK);
}

// There are no matching standard IFCs and matching default IFCs.
// We should select the ASs from the default IFCs
TEST_F(AsChainTest, NoMatchingStandardIFCsWithDefaultIFCs)
{
  IFCConfiguration ifc_configuration(true, true, "");
  Ifcs ifcs = non_matching_ifcs(2, "sip:as1", "sip:as2");
  Ifcs default_ifcs = matching_ifcs(2, "sip:default_as1", "sip:default_as2");
  AsChain as_chain(_as_chain_table, SessionCase::Originating, "sip:5755550011@homedomain", true, 0, ifcs, NULL, NULL, ifc_configuration);
  as_chain._default_ifcs = default_ifcs.ifcs_list();
  AsChainLink as_chain_link(&as_chain, 0u);

  pjsip_tx_data* tdata = NULL;
  create_invite(&tdata);
  std::string server_name;

  pjsip_status_code rc = as_chain_link.on_initial_request(tdata->msg, server_name, 1u);
  EXPECT_EQ(server_name, "sip:default_as1");
  EXPECT_EQ(rc, PJSIP_SC_OK);

  as_chain_link = as_chain_link.next();
  rc = as_chain_link.on_initial_request(tdata->msg, server_name, 1u);
  EXPECT_EQ(server_name, "sip:default_as2");
  EXPECT_EQ(rc, PJSIP_SC_OK);

  as_chain_link = as_chain_link.next();
  rc = as_chain_link.on_initial_request(tdata->msg, server_name, 1u);
  EXPECT_EQ(server_name, "");
  EXPECT_EQ(rc, PJSIP_SC_OK);
}

// There are no matching standard IFCs and no matching default IFCs.
// We shouldn't select any ASs.
TEST_F(AsChainTest, NoMatchingStandardOrDefaultIFCs)
{
  IFCConfiguration ifc_configuration(true, false, "");
  Ifcs ifcs = non_matching_ifcs(2, "sip:as1", "sip:as2");
  Ifcs default_ifcs = non_matching_ifcs(2, "sip:default_as2", "sip:default_as2");
  AsChain as_chain(_as_chain_table, SessionCase::Originating, "sip:5755550011@homedomain", true, 0, ifcs, NULL, NULL, ifc_configuration);
  as_chain._default_ifcs = default_ifcs.ifcs_list();
  AsChainLink as_chain_link(&as_chain, 0u);

  pjsip_tx_data* tdata = NULL;
  create_invite(&tdata);
  std::string server_name;

  pjsip_status_code rc = as_chain_link.on_initial_request(tdata->msg, server_name, 1u);
  EXPECT_EQ(server_name, "");
  EXPECT_EQ(rc, PJSIP_SC_OK);
}

// There are no matching standard IFCs and no matching default IFCs.
// We shouldn't select any ASs, and we should have an error response.
TEST_F(AsChainTest, NoMatchingStandardOrDefaultIFCsWithReject)
{
  IFCConfiguration ifc_configuration(true, true, "");
  Ifcs ifcs = non_matching_ifcs(2, "sip:as1", "sip:as2");
  Ifcs default_ifcs = non_matching_ifcs(2, "sip:default_as2", "sip:default_as2");
  AsChain as_chain(_as_chain_table, SessionCase::Originating, "sip:5755550011@homedomain", true, 0, ifcs, NULL, NULL, ifc_configuration);
  as_chain._default_ifcs = default_ifcs.ifcs_list();
  AsChainLink as_chain_link(&as_chain, 0u);

  pjsip_tx_data* tdata = NULL;
  create_invite(&tdata);
  std::string server_name;

  pjsip_status_code rc = as_chain_link.on_initial_request(tdata->msg, server_name, 1u);
  EXPECT_EQ(server_name, "");
  EXPECT_EQ(rc, PJSIP_SC_BAD_REQUEST);
}

// There are matching standard IFCs, and a subset of these match the dummy
// application server.
// We should select the ASs from the standard IFCs that don't match the dummy
// AS
TEST_F(AsChainTest, MatchingStandardIFCDummyAppServer)
{
  // Create an Ifcs with three IFCs, pointing to AS1, AS2, and AS3, and create
  // the AS Chain with AS2 set up as a dummy application server.
  IFCConfiguration ifc_configuration(false, false, "sip:AS2");
  Ifcs ifcs = matching_ifcs(3, "sip:AS1", "sip:AS2", "sip:AS3");
  AsChain as_chain(_as_chain_table, SessionCase::Originating, "sip:5755550011@homedomain", true, 0, ifcs, NULL, NULL, ifc_configuration);
  AsChainLink as_chain_link(&as_chain, 0u);

  pjsip_tx_data* tdata = NULL;
  create_invite(&tdata);
  std::string server_name;

  // Get the first AS - this should be AS1
  pjsip_status_code rc = as_chain_link.on_initial_request(tdata->msg, server_name, 1u);
  EXPECT_EQ(server_name, "sip:AS1");
  EXPECT_EQ(rc, PJSIP_SC_OK);

  // Get the second AS - this should be AS3, AS2 has been skipped over
  as_chain_link = as_chain_link.next();
  rc = as_chain_link.on_initial_request(tdata->msg, server_name, 1u);
  EXPECT_EQ(server_name, "sip:AS3");
  EXPECT_EQ(rc, PJSIP_SC_OK);

  // Get a third AS - this should be empty
  as_chain_link = as_chain_link.next();
  rc = as_chain_link.on_initial_request(tdata->msg, server_name, 1u);
  EXPECT_EQ(server_name, "");
  EXPECT_EQ(rc, PJSIP_SC_OK);
}

// There are matching standard IFCs, and all of these match the dummy
// application server.
// We shouldn't select any ASs.
TEST_F(AsChainTest, MatchingStandardIFCOnlyDummyAppServer)
{
  // Create an Ifcs with three IFCs, pointing to AS1, AS2, and AS3, and create
  // the AS Chain with AS2 set up as a dummy application server.
  IFCConfiguration ifc_configuration(false, false, "sip:dummy_as");
  Ifcs ifcs = matching_ifcs(1, "sip:dummy_as");
  AsChain as_chain(_as_chain_table, SessionCase::Originating, "sip:5755550011@homedomain", true, 0, ifcs, NULL, NULL, ifc_configuration);
  AsChainLink as_chain_link(&as_chain, 0u);

  pjsip_tx_data* tdata = NULL;
  create_invite(&tdata);
  std::string server_name;

  pjsip_status_code rc = as_chain_link.on_initial_request(tdata->msg, server_name, 1u);
  EXPECT_EQ(server_name, "");
  EXPECT_EQ(rc, PJSIP_SC_OK);
}

// There are matching standard IFCs, and all of these match the dummy
// application server. We reject if there are no matching IFCs
// We shouldn't select any ASs. This is still a success response.
TEST_F(AsChainTest, MatchingStandardIFCOnlyDummyAppServerWithReject)
{
  // Create an Ifcs with three IFCs, pointing to AS1, AS2, and AS3, and create
  // the AS Chain with AS2 set up as a dummy application server.
  IFCConfiguration ifc_configuration(false, true, "sip:dummy_as");
  Ifcs ifcs = matching_ifcs(1, "sip:dummy_as");
  AsChain as_chain(_as_chain_table, SessionCase::Originating, "sip:5755550011@homedomain", true, 0, ifcs, NULL, NULL, ifc_configuration);
  AsChainLink as_chain_link(&as_chain, 0u);

  pjsip_tx_data* tdata = NULL;
  create_invite(&tdata);
  std::string server_name;

  pjsip_status_code rc = as_chain_link.on_initial_request(tdata->msg, server_name, 1u);
  EXPECT_EQ(server_name, "");
  EXPECT_EQ(rc, PJSIP_SC_OK);
}

// There are matching standard IFCs, and all of these match the dummy
// application server. There are also matching default IFCs.
// We shouldn't select any ASs. This is still a success response.
TEST_F(AsChainTest, MatchingStandardIFCOnlyDummyAppServerWithDefaultIFCs)
{
  // Create an Ifcs with three IFCs, pointing to AS1, AS2, and AS3, and create
  // the AS Chain with AS2 set up as a dummy application server.
  IFCConfiguration ifc_configuration(true, true, "sip:dummy_as");
  Ifcs ifcs = matching_ifcs(1, "sip:dummy_as");
  Ifcs default_ifcs = matching_ifcs(1, "sip:default_as1");
  AsChain as_chain(_as_chain_table, SessionCase::Originating, "sip:5755550011@homedomain", true, 0, ifcs, NULL, NULL, ifc_configuration);
  as_chain._default_ifcs = default_ifcs.ifcs_list();
  AsChainLink as_chain_link(&as_chain, 0u);

  pjsip_tx_data* tdata = NULL;
  create_invite(&tdata);
  std::string server_name;

  pjsip_status_code rc = as_chain_link.on_initial_request(tdata->msg, server_name, 1u);
  EXPECT_EQ(server_name, "");
  EXPECT_EQ(rc, PJSIP_SC_OK);
}
