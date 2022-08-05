#include "router.hpp"
#include "doctest.h"
#include "server_commands.hpp"
#include "string_utils.hpp"
#include "text_data.hpp"

#include <tuple>

Router::Router()
    : m_controllers({
          {ServerCommand::List, Controllers::list},
          {ServerCommand::Create, Controllers::create},
          {ServerCommand::Login, Controllers::login},
          {ServerCommand::Search, Controllers::search},
          {ServerCommand::Add, Controllers::add},
          {ServerCommand::Remove, Controllers::remove},
          {ServerCommand::Ping, Controllers::ping},
          {ServerCommand::Available, Controllers::available},
          {ServerCommand::None, Controllers::none}, // this when calling unexisting command
      }){};

void Router::route(Request &t_req) {

  auto [command, arguments] = parse(t_req);

  if (validate_argument(command, arguments)) {
    m_controllers[command](arguments, t_req);

  } else {
    std::string reply = Reply::get_message(Reply::r_501);

    t_req.set_data(new TextData(reply));
  }
}

Router::CmdTuple Router::parse(Request &t_req) {

  std::string raw_msg = TextData::to_string(t_req.data());
  auto [command, arguments] = StringUtils::split_first(raw_msg);

  return std::make_tuple(ServerCommand::get(command), arguments);
}

bool Router::validate_argument(ServerCommand::name t_cmd, std::string &t_arg) {

  if (ServerCommand::has_argument(t_cmd)) {
    return !t_arg.empty();
  } else {

    return true;
  }
}

/** TESTS **/

TEST_CASE(
    "Router & Controllers (Postgres must be install to run these tests)") {

  Router router;
  Postgres pg; // just to check was written to database
               //
  SUBCASE("LIST Users contacts") {
    Request req;
    req.m_address = "123.453.3.1"; // must match user IP
    req.set_data(new TextData("LIST"));

    router.route(req);

    std::string expected(
        "201 id:2,username:mario,password:,online:false,address:1.453.32.1 "
        "id:3,username:shakira,password:,online:false,address:53.423.4.1");
    auto reply = TextData::to_string(req.data());
    CHECK(reply == expected);
  }

  SUBCASE("CREATE a user") {

    Request req;
    req.m_address = "127.0.0.1";
    req.set_data(new TextData("CREATE pedro 1234"));

    // route to the correct controler Controllers::create
    router.route(req);

    auto reply = TextData::to_string(req.data());
    CHECK(reply == Reply::get_message(Reply::r_200));

    User u = pg.search_user_by("pedro", "username");
    CHECK(u.username() == "pedro");

    // remove tests entry from the database
    pg.remove_user(u);
  }

  SUBCASE("LOGIN user") {

    User u = User(0, "marcos", "1234", true, "127.0.0.1");
    pg.add_user(u); // add user for testing and not mess around with seed data.

    Request req;
    req.m_address = "127.0.0.1"; //  must match
    req.set_data(new TextData("LOGIN marcos 1234"));
    router.route(req);

    auto reply = TextData::to_string(req.data());
    CHECK(reply == Reply::get_message(Reply::r_200));

    User marcos = pg.search_user_by("marcos", "username");
    pg.remove_user(marcos);
  }

  SUBCASE("Invalid LOGIN") {

    Request req;
    req.m_address = "127.0.0.1";
    req.set_data(new TextData("LOGIN shakira wrong_pass"));
    router.route(req);

    auto reply = TextData::to_string(req.data());
    CHECK(reply == Reply::get_message(Reply::r_300));
  }

  SUBCASE("SEARCH user") {

    Request req;
    req.set_data(new TextData("SEARCH mario"));
    router.route(req);

    auto reply = TextData::to_string(req.data());

    CHECK(reply == Reply::append_message(Reply::r_201, "id:2,username:mario,password:1234,online:false,address:1.453.32.1"));
  }

  SUBCASE("SEARCH unexisting user") {

    Request req;
    req.set_data(new TextData("SEARCH abdulah"));
    router.route(req);

    auto reply = TextData::to_string(req.data());
  
    CHECK(reply == Reply::get_message(Reply::r_301));
  }

  SUBCASE("ADD and REMOVE User contact") {

    // add
    Request req;
    req.m_address =
        "33.53.3.1"; // ip for dubius. must match in the database to find user.
    req.set_data(new TextData("ADD shakira")); // shakira is now dubius friend.
    router.route(req);

    auto reply = TextData::to_string(req.data());
    CHECK(reply == Reply::get_message(Reply::r_200));

    User dubius = pg.search_user_by("dubius", "username");
    Users contacts = pg.list_user_contacts(dubius);

    // dubius now have a contacts
    CHECK(contacts.empty() == false);
    CHECK(contacts.at(0).to_string() ==
          "id:3,username:shakira,password:,online:false,address:53.423.4.1");

    //  remove
    req.set_data(new TextData("REMOVE shakira"));
    router.route(req);

    reply = TextData::to_string(req.data());
    CHECK(reply == Reply::get_message(Reply::r_200));

    dubius = pg.search_user_by("dubius", "username");
    contacts = pg.list_user_contacts(dubius);

    // dubius has no friends.
    CHECK(contacts.empty() == true);
  }

  SUBCASE("ADD unexisting contact") {
    Request req;
    req.m_address = "33.53.3.1";                    // dubius IP address
    req.set_data(new TextData("ADD I_dont_exist")); //
    router.route(req);

    auto reply = TextData::to_string(req.data());
    CHECK(reply == Reply::get_message(Reply::r_301));
  }

  SUBCASE("REMOVE unexisting contact") {
    Request req;
    req.m_address = "33.53.3.1";                       // dubius IP address
    req.set_data(new TextData("REMOVE I_dont_exist")); //
    router.route(req);

    auto reply = TextData::to_string(req.data());
    CHECK(reply == Reply::get_message(Reply::r_301));
  }

  SUBCASE("PING user") {

    // create an online users. Test data only has offline users
    User user(0, "pedro", "1234", true, "1.1.1.1");
    pg.add_user(user);

    Request req;
    req.m_address = "1.1.1.1"; // new user IP address
    req.set_data(new TextData("PING"));

    router.route(req);

    auto reply = TextData::to_string(req.data());
    CHECK(reply == Reply::get_message(Reply::r_200));

    // clean up
    user = pg.search_user_by("pedro", "username");
    pg.remove_user(user);
  }

  SUBCASE("PING offline user") {
    Request req;
    req.m_address = "123.453.3.1"; // khalil IP address
    req.set_data(new TextData("PING"));

    router.route(req);

    auto reply = TextData::to_string(req.data());
    CHECK(reply == Reply::get_message(Reply::r_202));
  }

  SUBCASE("PING unexisting user") {
    Request req;
    req.m_address = "0.0.0.0"; // not  unexisting IP address
    req.set_data(new TextData("PING"));

    router.route(req);

    auto reply = TextData::to_string(req.data());
    CHECK(reply == Reply::get_message(Reply::r_301));
  }

  SUBCASE("AVAILABLE online user") {

    User user(0, "pedro", "1234", true, "1.1.1.1");
    pg.add_user(user);

    Request req;
    req.set_data(new TextData("AVAILABLE pedro"));

    router.route(req);

    auto reply = TextData::to_string(req.data());

    auto[code, msg] = StringUtils::split_first(reply);
    std::string message = msg;

   // msg will change everytime because of postgres will increment the id as we add in new use to test
   CHECK(reply == Reply::append_message(Reply::r_201, message)); 

    // clean up
    user = pg.search_user_by("pedro", "username");
    pg.remove_user(user);
  }

  SUBCASE("AVAILABLE offline user") {

    Request req;
    req.set_data(new TextData("AVAILABLE khalil"));

    router.route(req);

    auto reply = TextData::to_string(req.data());
    CHECK(reply == Reply::get_message(Reply::r_300));

  }

  SUBCASE("AVAILABLE unexisting user") {

    Request req;
    req.set_data(new TextData("AVAILABLE jonas"));

    router.route(req);

    auto reply = TextData::to_string(req.data());
    CHECK(reply == Reply::get_message(Reply::r_301));
  }

  SUBCASE("Bad command") {
    Request req;
    req.set_data(new TextData("BAD COMMAND"));
    router.route(req);

    auto reply = TextData::to_string(req.data());
    CHECK(reply == Reply::get_message(Reply::r_500));
  }
}
