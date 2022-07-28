#include "client.hpp"
#include "user.hpp"
#include "contacts.hpp"
#include "active_conn.hpp"
#include "passive_conn.hpp"

Client::Client(int port) : server_conn(port, new TextIO()) {}

bool Client::ping() { return true; }

