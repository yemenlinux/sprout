TARGETS := sprout cdiv_as.so gemini_as.so memento_as.so sprout_bgcf.so sprout_icscf.so sprout_mmtel_as.so sprout_scscf.so

TEST_TARGETS := sprout_test

SPROUT_COMMON_SOURCES := logger.cpp \
                         saslogger.cpp \
                         utils.cpp \
                         analyticslogger.cpp \
                         stack.cpp \
                         dnsparser.cpp \
                         dnscachedresolver.cpp \
                         baseresolver.cpp \
                         sipresolver.cpp \
                         bono.cpp \
                         registration_utils.cpp \
                         registrar.cpp \
                         authentication.cpp \
                         options.cpp \
                         connection_pool.cpp \
                         flowtable.cpp \
                         httpconnection.cpp \
                         httpresolver.cpp \
                         hssconnection.cpp \
                         websockets.cpp \
                         localstore.cpp \
                         memcachedstore.cpp \
                         memcachedstoreview.cpp \
                         memcached_config.cpp \
                         avstore.cpp \
                         subscriber_data_manager.cpp \
                         xdmconnection.cpp \
                         simservs.cpp \
                         enumservice.cpp \
                         bgcfservice.cpp \
                         icscfrouter.cpp \
                         scscfselector.cpp \
                         dnsresolver.cpp \
                         log.cpp \
                         pjutils.cpp \
                         statistic.cpp \
                         zmq_lvc.cpp \
                         trustboundary.cpp \
                         sessioncase.cpp \
                         ifchandler.cpp \
                         aschain.cpp \
                         custom_headers.cpp \
                         accumulator.cpp \
                         connection_tracker.cpp \
                         quiescing_manager.cpp \
                         dialog_tracker.cpp \
                         load_monitor.cpp \
                         counter.cpp \
                         basicproxy.cpp \
                         acr.cpp \
                         signalhandler.cpp \
                         health_checker.cpp \
                         subscription.cpp \
                         notify_utils.cpp \
                         unique.cpp \
                         chronosconnection.cpp \
                         accesslogger.cpp \
                         httpstack.cpp \
                         httpstack_utils.cpp \
                         handlers.cpp \
                         ipv6utils.cpp \
                         contact_filtering.cpp \
                         sproutletproxy.cpp \
                         pluginloader.cpp \
                         alarm.cpp \
                         base_communication_monitor.cpp \
                         communicationmonitor.cpp \
                         thread_dispatcher.cpp \
                         common_sip_processing.cpp \
                         exception_handler.cpp \
                         snmp_agent.cpp \
                         snmp_continuous_accumulator_table.cpp \
                         snmp_event_accumulator_table.cpp \
                         sip_string_to_request_type.cpp \
                         snmp_row.cpp \
                         snmp_scalar.cpp \
                         ralf_processor.cpp \
                         uri_classifier.cpp \
                         namespace_hop.cpp \
                         session_expires_helper.cpp \

sprout_SOURCES := ${SPROUT_COMMON_SOURCES} \
                  snmp_counter_table.cpp \
                  snmp_ip_count_table.cpp \
                  snmp_success_fail_count_table.cpp \
                  snmp_success_fail_count_by_request_type_table.cpp \
                  main.cpp

sprout_test_SOURCES := ${SPROUT_COMMON_SOURCES} \
                       mangelwurzel.cpp \
                       mobiletwinned.cpp \
                       scscfsproutlet.cpp \
                       icscfsproutlet.cpp \
                       bgcfsproutlet.cpp \
                       sproutletappserver.cpp \
                       mmtel.cpp \
                       test_main.cpp \
                       fakecurl.cpp \
                       fakehttpconnection.cpp \
                       fakexdmconnection.cpp \
                       fakehssconnection.cpp \
                       fakelogger.cpp \
                       faketransport_udp.cpp \
                       faketransport_tcp.cpp \
                       fakednsresolver.cpp \
                       fakechronosconnection.cpp \
                       basetest.cpp \
                       siptest.cpp \
                       sip_common.cpp \
                       sipresolver_test.cpp \
                       authentication_test.cpp \
                       simservs_test.cpp \
                       httpconnection_test.cpp \
                       hssconnection_test.cpp \
                       xdmconnection_test.cpp \
                       enumservice_test.cpp \
                       subscriber_data_manager_test.cpp \
                       avstore_test.cpp \
                       registrar_test.cpp \
                       bono_test.cpp \
                       bgcfservice_test.cpp \
                       options_test.cpp \
                       logger_test.cpp \
                       utils_test.cpp \
                       aschain_test.cpp \
                       sessioncase_test.cpp \
                       ifchandler_test.cpp \
                       custom_headers_test.cpp \
                       connection_tracker_test.cpp \
                       quiescing_manager_test.cpp \
                       dialog_tracker_test.cpp \
                       flow_test.cpp \
                       load_monitor_test.cpp \
                       icscfsproutlet_test.cpp \
                       basicproxy_test.cpp \
                       scscfselector_test.cpp \
                       acr_test.cpp \
                       subscription_test.cpp \
                       chronosconnection_test.cpp \
                       handlers_test.cpp \
                       httpresolver_test.cpp \
                       mock_sas.cpp \
                       contact_filtering_test.cpp \
                       appserver_test.cpp \
                       scscf_test.cpp \
                       sproutletproxy_test.cpp \
                       gruu_test.cpp \
                       mobiletwinned_test.cpp \
                       mangelwurzel_test.cpp \
                       alarm_test.cpp \
                       communicationmonitor_test.cpp \
                       common_sip_processing_test.cpp \
                       fakesnmp.cpp \
                       fakezmq.cpp \
                       uriclassifier_test.cpp \
                       ralf_processor_test.cpp \
                       mockhttpconnection.cpp \
                       session_expires_helper_test.cpp \
                       mock_subscriber_data_manager.cpp \
                       mock_chronos_connection.cpp \

SPROUT_COMMON_CPPFLAGS := -Wno-write-strings \
                          -I../include \
                          -I../modules/cpp-common/include \
                          -I../modules/app-servers/include \
                          -I../usr/include \
                          -I../modules/rapidjson/include \
                          `PKG_CONFIG_PATH=../usr/lib/pkgconfig pkg-config --cflags libpjproject`

sprout_CPPFLAGS := ${SPROUT_COMMON_CPPFLAGS}
sprout_test_CPPFLAGS := ${SPROUT_COMMON_CPPFLAGS} \
                        -I../modules/sipp \
                        -I../modules/app-servers/test \
                        -I../modules/gemini/include \
                        -I../include/mangelwurzel \
                        -Iut

SPROUT_COMMON_LDFLAGS := -rdynamic \
                         -L../usr/lib \
                         -lmemcached \
                         -lmemcachedutil \
                         -lssl \
                         -lcrypto \
                         -ldl \
                         -lwebsocketpp \
                         -lboost_regex \
                         -lboost_system \
                         -lboost_thread \
                         -lboost_date_time \
                         -lcares \
                         -lzmq \
                         -levhtp \
                         -levent \
                         -levent_pthreads \
                         -lcurl \
                         -lsas \
                         -lz \
                         -lpthread \
                         -lboost_filesystem \
                         `net-snmp-config --netsnmp-agent-libs`

# Explicitly link some pjsip modules. Some plugins require symbols in them
# (which sprout-base doesn't), and the plugins are dynamically linked at run
# time, so GCC won't link in the symbols they need unless we explicitly tell
# it to.
sprout_LDFLAGS := ${SPROUT_COMMON_LDFLAGS} -Wl,--whole-archive -lpjmedia-x86_64-unknown-linux-gnu -Wl,--no-whole-archive `PKG_CONFIG_PATH=../usr/lib/pkgconfig pkg-config --libs libpjproject`
sprout_test_LDFLAGS := ${SPROUT_COMMON_LDFLAGS} `PKG_CONFIG_PATH=../usr/lib/pkgconfig pkg-config --libs libpjproject`

# Build rules for sproutlet plugins
PLUGIN_COMMON_CPPFLAGS := -fPIC \
                          -I../include \
                          -I../modules/cpp-common/include \
                          -I../modules/app-servers/include \
                          -I../modules/rapidjson/include \
                          `PKG_CONFIG_PATH=../usr/lib/pkgconfig pkg-config --cflags libpjproject`
PLUGIN_COMMON_LDFLAGS := -shared -L../usr/lib

gemini_as.so_SOURCES := mobiletwinned.cpp sproutletappserver.cpp geminiasplugin.cpp
gemini_as.so_CPPFLAGS := ${PLUGIN_COMMON_CPPFLAGS} -I../modules/gemini/include
gemini_as.so_LDFLAGS := ${PLUGIN_COMMON_LDFLAGS}

memento_as.so_SOURCES := sproutletappserver.cpp mementoasplugin.cpp cassandra_store.cpp call_list_store.cpp mementosaslogger.cpp call_list_store_processor.cpp httpnotifier.cpp mementoappserver.cpp
memento_as.so_CPPFLAGS := ${PLUGIN_COMMON_CPPFLAGS} -I../modules/memento/include
memento_as.so_LDFLAGS := ${PLUGIN_COMMON_LDFLAGS} -lthrift -lcassandra

cdiv_as.so_SOURCES := mmtel.cpp sproutletappserver.cpp cdivasplugin.cpp
cdiv_as.so_CPPFLAGS := ${PLUGIN_COMMON_CPPFLAGS} -Wno-write-strings
cdiv_as.so_LDFLAGS := ${PLUGIN_COMMON_LDFLAGS}

sprout_bgcf.so_SOURCES := bgcfsproutlet.cpp bgcfservice.cpp bgcfplugin.cpp
sprout_bgcf.so_CPPFLAGS := ${PLUGIN_COMMON_CPPFLAGS}
sprout_bgcf.so_LDFLAGS := ${PLUGIN_COMMON_LDFLAGS}

sprout_icscf.so_SOURCES := icscfsproutlet.cpp icscfrouter.cpp scscfselector.cpp icscfplugin.cpp
sprout_icscf.so_CPPFLAGS := ${PLUGIN_COMMON_CPPFLAGS}
sprout_icscf.so_LDFLAGS := ${PLUGIN_COMMON_LDFLAGS}

sprout_mmtel_as.so_SOURCES := mmtel.cpp sproutletappserver.cpp mmtelasplugin.cpp
sprout_mmtel_as.so_CPPFLAGS := ${PLUGIN_COMMON_CPPFLAGS} -Wno-write-strings
sprout_mmtel_as.so_LDFLAGS := ${PLUGIN_COMMON_LDFLAGS}

sprout_scscf.so_SOURCES := scscfsproutlet.cpp scscfplugin.cpp
sprout_scscf.so_CPPFLAGS := ${PLUGIN_COMMON_CPPFLAGS}
sprout_scscf.so_LDFLAGS := ${PLUGIN_COMMON_LDFLAGS}

VPATH = ../modules/cpp-common/src ../modules/cpp-common/test_utils ../modules/app-servers/test ../modules/gemini/src ../modules/gemini/src/ut ../modules/memento/src mangelwurzel mangelwurzel/ut ut

include ../build-infra/cpp.mk

# Special extra objects for sprout_test
${BUILD_DIR}/bin/sprout_test : ${sprout_test_OBJECT_DIR}/md5.o \
                               ${sprout_test_OBJECT_DIR}/test_interposer.so

# Build rules for SIPp cryptographic modules
SIPP_DIR := ../modules/sipp
$(sprout_test_OBJECT_DIR)/md5.o : $(SIPP_DIR)/md5.c
	g++ $(sprout_test_CPPFLAGS) -I$(SIPP_DIR) -c $(SIPP_DIR)/md5.c -o $@

# Build rule for our interposer.
${sprout_test_OBJECT_DIR}/test_interposer.so : ../modules/cpp-common/test_utils/test_interposer.cpp ../modules/cpp-common/test_utils/test_interposer.hpp
	g++ $(sprout_test_CPPFLAGS) -shared -fPIC -ldl $< -o $@