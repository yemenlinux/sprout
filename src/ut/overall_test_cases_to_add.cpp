/**
 * @file XXXXXX
 *
 * Copyright (C) Metaswitch Networks 2017
 * If license terms are provided to you in a COPYING file in the root directory
 * of the source code repository by which you are accessing this code, then
 * the license outlined in that COPYING file applies to your use.
 * Otherwise no rights are granted except for those provided to you by
 * Metaswitch Networks in a separate written agreement.
 */

// Test that a call from an IMPU that belongs to a barred wildcarded public
// identity is rejected with a 403 (forbidden). The IMPU isn't included as
// a non-distinct IMPU in the HSS response.
TEST_F(SCSCFTest, TestBarredWildcardCaller)
{
  SCOPED_TRACE("");

  ServiceProfileBuilder service_profile = ServiceProfileBuilder()
    .addIdentity("sip:610@homedomain")
    .addIdentity("sip:65!.*!@homedomain")
    .addBarringIndication("sip:65!.*!@homedomain", "1")
    .addIfc(1, {"<Method>INVITE</Method>"}, "sip:1.2.3.4:56789;transport=UDP");
  SubscriptionBuilder subscription = SubscriptionBuilder()
    .addServiceProfile(service_profile);

  _hss_connection->set_impu_result("sip:6505551000@homedomain",
                                   "call",
                                   "REGISTERED",
                                   subscription.return_sub());

  SCSCFMessage msg;
  msg._route = "Route: <sip:sprout.homedomain;orig>";
  doSlowFailureFlow(msg, 403);
}

// Test that a call to an IMPU that belongs to a barred wildcarded public
// identity is rejected with a 404 (not found). The IMPU isn't included as
// a non-distinct IMPU in the HSS response.
TEST_F(SCSCFTest, TestBarredWildcardCallee)
{
  SCOPED_TRACE("");

  ServiceProfileBuilder service_profile = ServiceProfileBuilder()
    .addIdentity("sip:610@homedomain")
    .addIdentity("sip:65!.*!@homedomain")
    .addBarringIndication("sip:65!.*!@homedomain", "1")
    .addIfc(1, {"<Method>INVITE</Method>"}, "sip:1.2.3.4:56789;transport=UDP");
  SubscriptionBuilder subscription = SubscriptionBuilder()
    .addServiceProfile(service_profile);

  _hss_connection->set_impu_result("sip:6505551000@homedomain",
                                   "call",
                                   "REGISTERED",
                                   subscription.return_sub());

  SCSCFMessage msg;
  doSlowFailureFlow(msg, 404);
}

TEST_F(SCSCFTest, TestWildcardBarredCaller)
{
  SCOPED_TRACE("");

  ServiceProfileBuilder service_profile = ServiceProfileBuilder()
    .addIdentity("sip:610@homedomain")
    .addIdentity("sip:65!.*!@homedomain")
    .addIdentity("sip:6505551000@homedomain")
    .addBarringIndication("sip:6505551000@homedomain", "1")
    .addWildcard("sip:6505551000@homedomain", 3, "sip:65!.*!@homedomain")
    .addIfc(1, {"<Method>INVITE</Method>"}, "sip:1.2.3.4:56789;transport=UDP");
  SubscriptionBuilder subscription = SubscriptionBuilder()
    .addServiceProfile(service_profile);

  _hss_connection->set_impu_result("sip:6505551000@homedomain",
                                   "call",
                                   "REGISTERED",
                                   subscription.return_sub());

  SCSCFMessage msg;
  msg._route = "Route: <sip:sprout.homedomain;orig>";
  doSlowFailureFlow(msg, 403);
}

// Test that a call to a barred IMPU that belongs to a non-barred wildcarded
// public identity is rejected with a 404. The IMPU is included as a
// non-distinct IMPU in the HSS response.
TEST_F(SCSCFTest, TestWildcardBarredCallee)
{
  SCOPED_TRACE("");

  ServiceProfileBuilder service_profile = ServiceProfileBuilder()
    .addIdentity("sip:610@homedomain")
    .addIdentity("sip:65!.*!@homedomain")
    .addIdentity("sip:6505551000@homedomain")
    .addBarringIndication("sip:6505551000@homedomain", "1")
    .addWildcard("sip:6505551000@homedomain", 3, "sip:65!.*!@homedomain")
    .addIfc(1, {"<Method>INVITE</Method>"}, "sip:1.2.3.4:56789;transport=UDP");
  SubscriptionBuilder subscription = SubscriptionBuilder()
    .addServiceProfile(service_profile);

  _hss_connection->set_impu_result("sip:6505551000@homedomain",
                                   "call",
                                   "REGISTERED",
                                   subscription.return_sub());

  SCSCFMessage msg;
  doSlowFailureFlow(msg, 404);
}

// Test that a call to a barred IMPU that belongs to a non-barred wildcarded
// public identity is rejected with a 404. The HSS response includes multiple
// wildcard identities that could match the IMPU, so this checks that the
// correct identity is selected.
TEST_F(SCSCFTest, TestBarredMultipleWildcardCallee)
{
  SCOPED_TRACE("");

  ServiceProfileBuilder service_profile_1 = ServiceProfileBuilder()
    .addIdentity("sip:610@homedomain")
    .addIdentity("sip:6!.*!@homedomain")
    .addIfc(1, {"<Method>INVITE</Method>"}, "sip:1.2.3.4:56789;transport=UDP");
  ServiceProfileBuilder service_profile_2 = ServiceProfileBuilder()
    .addIdentity("sip:611@homedomain")
    .addIdentity("sip:65!.*!@homedomain")
    .addIdentity("650!.*!@homedomain")
    .addIdentity("sip:6505551000@homedomain")
    .addBarringIndication("sip:6505551000@homedomain", "1")
    .addWildcard("sip:6505551000@homedomain", 3, "sip:65!.*!@homedomain")
    .addIfc(1, {"<Method>INVITE</Method>"}, "sip:1.2.3.4:56789;transport=UDP");
  ServiceProfileBuilder service_profile_3 = ServiceProfileBuilder()
    .addIdentity("sip:612@homedomain")
    .addIdentity("sip:!.*!@homedomain")
    .addIfc(1, {"<Method>INVITE</Method>"}, "sip:1.2.3.4:56789;transport=UDP");
  SubscriptionBuilder subscription = SubscriptionBuilder()
    .addServiceProfile(service_profile_1)
    .addServiceProfile(service_profile_2)
    .addServiceProfile(service_profile_3);

  _hss_connection->set_impu_result("sip:6505551000@homedomain",
                                   "call",
                                   "REGISTERED",
                                   subscription.return_sub());
  SCSCFMessage msg;
  doSlowFailureFlow(msg, 404);
}

// Test that a call from a barred IMPU that belongs to a non-barred wildcarded
// public identity is rejected with a 403. The HSS response includes multiple
// wildcard identities that could match the IMPU, so this checks that the
// correct identity is selected.
TEST_F(SCSCFTest, TestBarredMultipleWildcardCaller)
{
  SCOPED_TRACE("");

  ServiceProfileBuilder service_profile_1 = ServiceProfileBuilder()
    .addIdentity("sip:610@homedomain")
    .addIdentity("sip:6!.*!@homedomain")
    .addIfc(1, {"<Method>INVITE</Method>"}, "sip:1.2.3.4:56789;transport=UDP");
  ServiceProfileBuilder service_profile_2 = ServiceProfileBuilder()
    .addIdentity("sip:611@homedomain")
    .addIdentity("sip:65!.*!@homedomain")
    .addIdentity("650!.*!@homedomain")
    .addIdentity("sip:6505551000@homedomain")
    .addBarringIndication("sip:6505551000@homedomain", "1")
    .addWildcard("sip:6505551000@homedomain", 3, "sip:65!.*!@homedomain")
    .addIfc(1, {"<Method>INVITE</Method>"}, "sip:1.2.3.4:56789;transport=UDP");
  ServiceProfileBuilder service_profile_3 = ServiceProfileBuilder()
    .addIdentity("sip:612@homedomain")
    .addIdentity("sip:!.*!@homedomain")
    .addIfc(1, {"<Method>INVITE</Method>"}, "sip:1.2.3.4:56789;transport=UDP");
  SubscriptionBuilder subscription = SubscriptionBuilder()
    .addServiceProfile(service_profile_1)
    .addServiceProfile(service_profile_2)
    .addServiceProfile(service_profile_3);

  _hss_connection->set_impu_result("sip:6505551000@homedomain",
                                   "call",
                                   "REGISTERED",
                                   subscription.return_sub());
  SCSCFMessage msg;
  msg._route = "Route: <sip:sprout.homedomain;orig>";
  doSlowFailureFlow(msg, 403);
}

TEST_F(SCSCFTest, TestTelURIWildcard)
{
  ServiceProfileBuilder service_profile = ServiceProfileBuilder()
    .addIdentity("tel:6505552345")
    .addIdentity("tel:65055522!.*!")
    .addIdentity("tel:65055512!.*!")
    .addIfc(1, {"<Method>INVITE</Method>"}, "sip:1.2.3.4:56789;transport=UDP");
  SubscriptionBuilder subscription = SubscriptionBuilder()
    .addServiceProfile(service_profile);
  _hss_connection->set_impu_result("tel:6505551235",
                                   "call",
                                   "REGISTERED",
                                   subscription.return_sub());

  TransportFlow tpBono(TransportFlow::Protocol::TCP, stack_data.scscf_port, "10.99.88.11", 12345);
  TransportFlow tpAS1(TransportFlow::Protocol::UDP, stack_data.scscf_port, "1.2.3.4", 56789);

  // Send a terminating INVITE for a subscriber with a tel: URI
  SCSCFMessage msg;
  msg._via = "10.99.88.11:12345;transport=TCP";
  msg._to = "6505551234@homedomain";
  msg._route = "Route: <sip:sprout.homedomain>";
  msg._todomain = "";
  msg._requri = "tel:6505551235";

  msg._method = "INVITE";
  list<HeaderMatcher> hdrs;

  inject_msg(msg.get_request(), &tpBono);
  poll();
  ASSERT_EQ(2, txdata_count());

  // 100 Trying goes back to bono
  pjsip_msg* out = current_txdata()->msg;
  RespMatcher(100).matches(out);
  tpBono.expect_target(current_txdata(), true);  // Requests always come back on same transport
  msg.convert_routeset(out);
  free_txdata();
  ASSERT_EQ(1, txdata_count());

  // INVITE passed on to AS1
  SCOPED_TRACE("INVITE (S)");
  pjsip_tx_data* tdata = current_txdata();
  out = tdata->msg;
  ReqMatcher r1("INVITE");
  ASSERT_NO_FATAL_FAILURE(r1.matches(out));

  tpAS1.expect_target(tdata, false);
  EXPECT_THAT(get_headers(out, "Route"),
              testing::MatchesRegex("Route: <sip:1\\.2\\.3\\.4:56789;transport=UDP;lr>\r\nRoute: <sip:odi_[+/A-Za-z0-9]+@127.0.0.1:5058;transport=UDP;lr;service=scscf>"));
  string fresp1 = respond_to_txdata(tdata, 404);
  inject_msg(fresp1, &tpAS1);
  ASSERT_EQ(3, txdata_count());
  free_txdata();
  free_txdata();
  ASSERT_EQ(1, txdata_count());

  // 100 Trying goes back to bono
  out = current_txdata()->msg;
  RespMatcher(404).matches(out);
  free_txdata();
  ASSERT_EQ(0, txdata_count());
}

TEST_F(SCSCFTest, TestMultipleServiceProfiles)
{
  ServiceProfileBuilder service_profile_1 = ServiceProfileBuilder()
    .addIdentity("tel:6505552345")
    .addIdentity("tel:65055512!.*!")
    .addIfc(1, {"<Method>INVITE</Method>"}, "sip:5.6.7.8:56789;transport=UDP");
  ServiceProfileBuilder service_profile_2 = ServiceProfileBuilder()
    .addIdentity("tel:6505551235")
    .addIfc(1, {"<Method>INVITE</Method>"}, "sip:1.2.3.4:56789;transport=UDP");
  SubscriptionBuilder subscription = SubscriptionBuilder()
    .addServiceProfile(service_profile_1)
    .addServiceProfile(service_profile_2);
  _hss_connection->set_impu_result("tel:6505551235",
                                   "call",
                                   "REGISTERED",
                                   subscription.return_sub());

  TransportFlow tpBono(TransportFlow::Protocol::TCP, stack_data.scscf_port, "10.99.88.11", 12345);
  TransportFlow tpAS1(TransportFlow::Protocol::UDP, stack_data.scscf_port, "1.2.3.4", 56789);

  // Send a terminating INVITE for a subscriber with a tel: URI
  SCSCFMessage msg;
  msg._via = "10.99.88.11:12345;transport=TCP";
  msg._to = "6505551234@homedomain";
  msg._route = "Route: <sip:sprout.homedomain>";
  msg._todomain = "";
  msg._requri = "tel:6505551235";

  msg._method = "INVITE";
  list<HeaderMatcher> hdrs;

  inject_msg(msg.get_request(), &tpBono);
  poll();
  ASSERT_EQ(2, txdata_count());

  // 100 Trying goes back to bono
  pjsip_msg* out = current_txdata()->msg;
  RespMatcher(100).matches(out);
  tpBono.expect_target(current_txdata(), true);  // Requests always come back on same transport
  msg.convert_routeset(out);
  free_txdata();
  ASSERT_EQ(1, txdata_count());

  // INVITE passed on to AS1
  SCOPED_TRACE("INVITE (S)");
  pjsip_tx_data* tdata = current_txdata();
  out = tdata->msg;
  ReqMatcher r1("INVITE");
  ASSERT_NO_FATAL_FAILURE(r1.matches(out));

  tpAS1.expect_target(tdata, false);
  EXPECT_THAT(get_headers(out, "Route"),
              -              testing::MatchesRegex("Route: <sip:1\\.2\\.3\\.4:56789;transport=UDP;lr>\r\nRoute: <sip:odi_[+/A-Za-z0-9]+@127.0.0.1:5058;transport=UDP;lr;service=scscf>"));
  string fresp1 = respond_to_txdata(tdata, 404);
  inject_msg(fresp1, &tpAS1);
  ASSERT_EQ(3, txdata_count());
  free_txdata();
  free_txdata();
  ASSERT_EQ(1, txdata_count());

  // 100 Trying goes back to bono
  out = current_txdata()->msg;
  RespMatcher(404).matches(out);
  free_txdata();
  ASSERT_EQ(0, txdata_count());
}

TEST_F(SCSCFTest, TestMultipleAmbiguousServiceProfiles)
{
  ServiceProfileBuilder service_profile_1 = ServiceProfileBuilder()
    .addIdentity("tel:6505552345")
    .addIdentity("tel:65055512!.*!")
    .addIfc(1, {"<Method>INVITE</Method>"}, "sip:1.2.3.4:56789;transport=UDP");
  ServiceProfileBuilder service_profile_2 = ServiceProfileBuilder()
    .addIdentity("tel:650555123!.*!")
    .addIfc(1, {"<Method>INVITE</Method>"}, "sip:5.6.7.8:56789;transport=UDP");
  SubscriptionBuilder subscription = SubscriptionBuilder()
    .addServiceProfile(service_profile_1)
    .addServiceProfile(service_profile_2);
  _hss_connection->set_impu_result("tel:6505551235",
                                   "call",
                                   "REGISTERED",
                                   subscription.return_sub());

  TransportFlow tpBono(TransportFlow::Protocol::TCP, stack_data.scscf_port, "10.99.88.11", 12345);
  TransportFlow tpAS1(TransportFlow::Protocol::UDP, stack_data.scscf_port, "1.2.3.4", 56789);

  // Send a terminating INVITE for a subscriber with a tel: URI
  SCSCFMessage msg;
  msg._via = "10.99.88.11:12345;transport=TCP";
  msg._to = "6505551234@homedomain";
  msg._route = "Route: <sip:sprout.homedomain>";
  msg._todomain = "";
  msg._requri = "tel:6505551235";

  msg._method = "INVITE";
  list<HeaderMatcher> hdrs;

  inject_msg(msg.get_request(), &tpBono);
  poll();
  ASSERT_EQ(2, txdata_count());

  // 100 Trying goes back to bono
  pjsip_msg* out = current_txdata()->msg;
  RespMatcher(100).matches(out);
  tpBono.expect_target(current_txdata(), true);  // Requests always come back on same transport
  msg.convert_routeset(out);
  free_txdata();
  ASSERT_EQ(1, txdata_count());

  // INVITE passed on to AS1
  SCOPED_TRACE("INVITE (S)");
  pjsip_tx_data* tdata = current_txdata();
  out = tdata->msg;
  ReqMatcher r1("INVITE");
  ASSERT_NO_FATAL_FAILURE(r1.matches(out));

  tpAS1.expect_target(tdata, false);
  EXPECT_THAT(get_headers(out, "Route"),
              testing::MatchesRegex("Route: <sip:1\\.2\\.3\\.4:56789;transport=UDP;lr>\r\nRoute: <sip:odi_[+/A-Za-z0-9]+@127.0.0.1:5058;transport=UDP;lr;service=scscf>"));
  string fresp1 = respond_to_txdata(tdata, 404);
  inject_msg(fresp1, &tpAS1);
  ASSERT_EQ(3, txdata_count());
  free_txdata();
  free_txdata();
  ASSERT_EQ(1, txdata_count());

  // 100 Trying goes back to bono
  out = current_txdata()->msg;
  RespMatcher(404).matches(out);
  free_txdata();
  ASSERT_EQ(0, txdata_count());
}

