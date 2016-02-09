extern "C" {
	#include "conf_reader.h"
	#include "acceptor_eh.h"
	#include "protocol.h"
	#include "if_config.h"
	#include "client_eh.h"
	#include "reactor.h"
	//#include "port_configurator.h"
}

#include "mocks/os_mock.h"

#include <gtest/gtest.h>

using namespace ::testing;

TEST(server_test, epoll_wait_fails_first_time)
{
	int srv_fd = 1;
	int epoll_fd = 4;
	
	serv_sett ss;
	ss.port = 3000;
	ss.max_clients = 10;

	reactor *r = 0;
	event_handler* serv_eh = 0;

	os_epoll_create_mock ecrem;
	os_epoll_ctl_mock ecm;
	os_epoll_wait_mock ewm;
	
	os_socket_mock osm;
	os_bind_mock obm;
	os_listen_mock olm;

	EXPECT_FUNCTION_CALL(ecrem, (ss.max_clients+1)).WillOnce(Return(epoll_fd));
	EXPECT_FUNCTION_CALL(ecm, (epoll_fd, _, srv_fd, _)).WillOnce(Return(0));
	EXPECT_FUNCTION_CALL(ewm, (epoll_fd, _, _, -1)).WillOnce(Return(-1));

	EXPECT_FUNCTION_CALL(osm, (_,_,_)).WillOnce(Return(srv_fd));
	EXPECT_FUNCTION_CALL(obm, (srv_fd, _, _)).WillOnce(Return(0));
	EXPECT_FUNCTION_CALL(olm, (srv_fd, _)).WillOnce(Return(0));

	r = create_reactor(ss.max_clients);
	if (r == 0) {
		return;
	}

	serv_eh = construct_acceptor(r, &ss);
	if (serv_eh == 0) {
		destroy_reactor(r);
		return;
	}

	r->add_eh(r, serv_eh);
	r->event_loop(r);
	free(r);
	free(serv_eh);

	ASSERT_TRUE(true);
}

TEST(server_test, accept_client_success)
{
	int srv_fd = 1;
	int cli_fd = 2;
	int epoll_fd = 4;
	
	struct epoll_event e;
	e.events = EPOLLIN;
	e.data.fd = srv_fd;

	serv_sett ss;
	ss.port = 3000;
	ss.max_clients = 10;

	reactor *r = 0;
	event_handler* serv_eh = 0;

	os_epoll_create_mock ecrem;
	os_epoll_ctl_mock ecm;
	os_epoll_wait_mock ewm;
	
	os_socket_mock osm;
	os_bind_mock obm;
	os_listen_mock olm;
	os_accept_mock oam;

	EXPECT_FUNCTION_CALL(ecrem, (ss.max_clients+1)).WillOnce(Return(epoll_fd));
	EXPECT_FUNCTION_CALL(ecm, (epoll_fd, _, _, _)).Times(2)\
													.WillRepeatedly(Return(0));
	EXPECT_FUNCTION_CALL(ewm, (epoll_fd, _, _, -1)).Times(2)\
													.WillOnce(DoAll(SetArgPointee<1>(e), Return(1)))\
													.WillOnce(Return(-1));

	EXPECT_FUNCTION_CALL(osm, (_,_,_)).WillOnce(Return(srv_fd));
	EXPECT_FUNCTION_CALL(obm, (srv_fd, _, _)).WillOnce(Return(0));
	EXPECT_FUNCTION_CALL(olm, (srv_fd, _)).WillOnce(Return(0));
	EXPECT_FUNCTION_CALL(oam, (srv_fd, 0, 0)).WillOnce(Return(cli_fd));

	r = create_reactor(ss.max_clients);
	if (r == 0) {
		return;
	}

	serv_eh = construct_acceptor(r, &ss);
	if (serv_eh == 0) {
		destroy_reactor(r);
		return;
	}

	r->add_eh(r, serv_eh);
	r->event_loop(r);
	free(r);
	free(serv_eh);

	ASSERT_TRUE(true);
}


TEST(server_test, construct_acceptor_failed_socket)
{
	reactor *r = 0;
	event_handler *serv_eh;
	
	
	os_socket_mock osm;

	EXPECT_FUNCTION_CALL(osm, (_,_,_)).WillOnce(Return(-1));

	r = create_reactor(1);
	if (r == 0) {
		return;
	}

	serv_eh = construct_acceptor(r, NULL);
	if (serv_eh == 0) {
		destroy_reactor(r);
		return;
	}
	
	ASSERT_TRUE(true);
}

TEST(server_test, construct_acceptor_failed_bind)
{
	reactor *r = 0;
	event_handler *serv_eh;
	int serv_fd = 2;
	
	serv_sett ss;
	ss.port = 3000;
	ss.max_clients = 10;

	os_socket_mock osm;
	os_bind_mock obm;

	EXPECT_FUNCTION_CALL(osm, (_,_,_)).WillOnce(Return(serv_fd));
	EXPECT_FUNCTION_CALL(obm, (_,_,_)).WillOnce(Return(-1));

	r = create_reactor(1);
	if (r == 0) {
		return;
	}

	serv_eh = construct_acceptor(r, &ss);
	if (serv_eh == 0) {
		destroy_reactor(r);
		return;
	}
	
	ASSERT_TRUE(true);
}


TEST(server_test, construct_acceptor_failed_listen)
{
	reactor *r = 0;
	event_handler *serv_eh;
	int srv_fd = 2;
	
	serv_sett ss;
	ss.port = 3000;
	ss.max_clients = 10;

	os_socket_mock osm;
	os_bind_mock obm;
	os_listen_mock olm;

	EXPECT_FUNCTION_CALL(osm, (_,_,_)).WillOnce(Return(srv_fd));
	EXPECT_FUNCTION_CALL(obm, (srv_fd, _,_)).WillOnce(Return(0));
	EXPECT_FUNCTION_CALL(olm, (srv_fd, _)).WillOnce(Return(-1));

	r = create_reactor(1);
	if (r == 0) {
		return;
	}

	serv_eh = construct_acceptor(r, &ss);
	if (serv_eh == 0) {
		destroy_reactor(r);
		return;
	}
	
	ASSERT_TRUE(true);
}
