#include "AuthRoutes.h"

#include "vmc-libserver/Database.h"
#include "vmc-libserver/HTTPUtils.h"
#include "vmc-libserver/Auth.h"

using json = nlohmann::json;

namespace vmc
{
    namespace routes
    {
        namespace auth
        {
            void route(Router &router)
            {
                router.route({vmc::method::GET}, "/loginStatus",
                    [](HTTPRequest &request, std::vector<std::string> const &urlParts, std::unordered_map<std::string, std::string> const &urlParams) {
                        std::shared_ptr<Session> session = request.initSession();

                        json response = {{"loggedIn", false}};

                        if (session->exists("authenticated") && session->get<bool>("authenticated"))
                        {
                            response["loggedIn"] = true;
                            response["username"] = session->get<std::string>("username");
                            if (session->exists("email"))
                            {
                                response["email"] = session->get<std::string>("email");
                            }
                        }

                        util::sendJSON(request, response);
                    });

                router.route({vmc::method::POST}, "/login",
                    [](HTTPRequest &request, std::vector<std::string> const &urlParts, std::unordered_map<std::string, std::string> const &urlParams) {
                        if (!request.hasPostData() || !request.getPostData()->hasJsonData()) QUIT_BAD_REQUEST(request);

                        json requestJson = request.getPostData()->getJson();
                        if (requestJson.count("username") == 0) QUIT_BAD_REQUEST(request);

                        json response = {};
                        std::string user = requestJson["username"];
                        std::shared_ptr<Session> session = request.initSession();
                        if (user == "guest")
                        {
                            std::string displayName = "Guest";
                            if (requestJson.count("display-name") > 0)
                            {
                                displayName = requestJson["display-name"];
                            }
                            session->put("authenticated", true);
                            session->put("access-level", 1);
                            session->put("username", displayName);
                            response["okay"] = true;
                            response["user"] = {
                                {"username", displayName},
                                {"privilege", 1}
                            };
                        }
                        else
                        {
                            if (requestJson.count("password") == 0) QUIT_BAD_REQUEST(request);

                            auto db = database::getDatabase();
                            auto query = db->store("SELECT * FROM users WHERE username = %0q", {user});

                            if (!query || query.num_rows() == 0)
                            {
                                QUIT_MSG(request, 406, "{\"okay\": false, \"error\": \"Invalid login\"}");
                            }

                            auto userData = query[0];

                            auto hash = (const char *) userData["password"];
                            std::string password = requestJson["password"];
                            auto validPassword = vmc::auth::comparePassword(hash, password);

                            response["okay"] = validPassword;

                            if (validPassword)
                            {
                                response["user"] = {
                                    {"username", (const char *) userData["username"]},
                                    {"email", (const char *) userData["email"]},
                                    {"privilege", (int) userData["privilege"]}
                                };

                                session->put("authenticated", true);
                                session->put("access-level", (int) userData["privilege"]);
                                session->put<std::string>("username", (const char *) userData["username"]);
                                session->put<std::string>("email", (const char *) userData["email"]);
                            }
                        }

                        util::sendJSON(request, response);
                    });

                router.route({vmc::method::POST}, "/logout",
                    [](HTTPRequest &request, std::vector<std::string> const &urlParts, std::unordered_map<std::string, std::string> const &urlParams) {
                        auto session = request.initSession();
                        session->put("authenticated", false);
                        session->put("access-level", 0);
                        session->put<std::string>("username", "");
                        json response = {};
                        response["okay"] = true;
                        response["msg"] = "Logged out successfully";
                        util::sendJSON(request, response);
                    });
            }
        }
    }
}
