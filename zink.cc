#include <zmq.hpp>
#include <string>
#include <iostream>
#include <cstdlib>
#include <unistd.h>

static bool IsTrue(std::string s) { return s == "true" || s == "TRUE" || s == "1"; }

class Zink
{
    // Default configuration
    std::string endpoint;
    std::string public_key;
    std::string secret_key;
    bool curve;
    bool print_messages;
    bool print_parts;
    bool print_compact;

    int pause_after;
    int exit_after;

    zmq::context_t &context;
    zmq::socket_t socket;
    size_t message_cnt = 0;

  public:
    Zink(zmq::context_t &context)
            : endpoint("tcp://*:13415"),
              public_key("rq:rM>}U?@Lns47E1%kR.o@n%FcmmsL/@{H8]yf7"),
              secret_key("JTKVSB%%)wK0E.X)V>+}o?pNmC{O&4W4b!Ni{Lh6"),
              curve(false),
              print_messages(true),
              print_parts(false),
              print_compact(false),
              pause_after(-1),
              exit_after(-1),
              context(context),
              socket(context, ZMQ_SUB)
    {
        const char *env;
        if (env = std::getenv("ZINK_ENDPOINT"))
            endpoint = env;

        if (env = std::getenv("ZINK_CURVE"))
            curve = IsTrue(env);

        if (env = std::getenv("ZINK_SECRET_KEY"))
        {
            endpoint = env;
            curve = true;
        }

        if (env = std::getenv("ZINK_PRINT"))
        {
            if (std::string("PARTS") == env)
                print_parts = true;

            if (std::string("COMPACT") == env)
                print_compact = true;
        }

        if (env = std::getenv("ZINK_PAUSE_AFTER"))
            pause_after = std::stoul(env);

        if (env = std::getenv("ZINK_EXIT_AFTER"))
            exit_after = std::stoul(env);

        if (curve)
        {
            socket.setsockopt(ZMQ_CURVE_SECRETKEY, secret_key.c_str(), secret_key.length());
            int curve_server = 1;
            socket.setsockopt(ZMQ_CURVE_SERVER, &curve_server, sizeof(curve_server));
        }

        socket.bind(endpoint.c_str());
        socket.setsockopt(ZMQ_SUBSCRIBE, "", 0);
    }

    void ReceiveMessages()
    {
        while (true)
        {
            if (exit_after != -1 && message_cnt >= exit_after)
            {
                std::cout << std::endl << "Exiting after " << message_cnt << " messages"
                          << std::endl;
                return;
            }

            if (pause_after != -1 && message_cnt >= pause_after)
            {
                std::cout << std::endl << "Pausing after " << message_cnt << " messages"
                          << std::endl;
                pause();
                return;
            }

            ReceiveMessage();
        }
    }

    void ReceiveMessage()
    {
        bool more = true;
        int parts_cnt = 0;
        while (more)
        {
            zmq::message_t message;
            auto rc = socket.recv(&message);
            if (rc)
            {
                parts_cnt++;
                std::string m(reinterpret_cast<const char *>(message.data()), message.size());
                if (print_parts)
                    std::cout << "PART " << parts_cnt << ": " << m << std::endl;
                more = message.more();
            }
        }

        message_cnt++;

        if (print_compact)
        {
            std::cout << "." << std::flush;
            if (message_cnt % 100 == 0)
                std::cout << std::endl << message_cnt << " " << std::flush;
        }
        else if (print_messages)
            std::cout << "Message " << message_cnt << " with " << parts_cnt
                      << (parts_cnt > 1 ? " parts" : " part") << std::endl;
    }
};

int main(int argc, char **argv)
{
    zmq::context_t context(1);

    Zink z(context);
    z.ReceiveMessages();
}
