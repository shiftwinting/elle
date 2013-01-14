#ifndef GAP_ANALIZER_HH
# define GAP_ANALIZER_HH

# include <elle/types.hh>
# include <elle/print.hh>
# include <elle/HttpClient.hh>
# include <queue>
# include <map>

namespace surface
{
  namespace gap
  {
    class MetricReporter:
      private boost::noncopyable
    {
      /*------.
      | Types |
      `------*/
    public:
      typedef std::map<std::string, std::string> Metric;

      /*-------------.
      | Construction |
      `-------------*/
    public:
      /// Default constructor.
      //XXX: Ctor with choosable server.
      MetricReporter(std::string const& server,
                     uint16_t port);

      // Destroy analyzer after sending data to server or storing localy if
      // connection was lost.
      ~MetricReporter();

      /*--------.
      | Methods |
      `--------*/
    public:
      // Enqueue uri acces.
      void
      store(std::string const& name, Metric const&);

      void
      store(std::string const& caller);

      // Sugar to avoid store(name, {{key, value}}).
      void
      store(std::string const& name,
            std::string const& key,
            std::string const& value);

      // Sugar to generate a new transaction event.
      void
      store_transaction(std::string const& name, Metric const&);


      // Defaultly, user is anonymous.
      void
      update_user(std::string const&);

    private:
      void
      _send_data();

      /*-----------.
      | Attributes |
      `-----------*/
    private:
      std::queue<Metric> _requests;
      std::unique_ptr<elle::HttpClient> _server;
      std::string _user_id;
    };

    namespace metrics
    {
      namespace google
      {
        //XXX: create an enum and a map that list with a user friendly name the
        // key of the field such as Key::session_control instead of "sc"

        static const std::pair<std::string, std::string> Success{"cd1", "success"};
        static const std::pair<std::string, std::string> Failure{"cd1", "failure"};
        static const std::pair<std::string, std::string> Pending{"cd1", "pending"};

        // Create a static MetricReporter with google analytics server.
        MetricReporter&
        server();
      }
    }
  }
}

#endif
