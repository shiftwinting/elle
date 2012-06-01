
#include <cstdlib>
#include <iostream>
#include <string>
#include <stdexcept>

#include <QDir>


#include "Application.hh"

static void _initAll();

static bool check_dirty_demo_hack();

int     main(int ac, char* av[])
{
  if (check_dirty_demo_hack())
    {
      return 0;
    }

  try
    {
      _initAll();
      plasma::updater::Application app(ac, av);
      return app.Exec();
    }
  catch (std::exception const& err)
    {
      std::cerr << "A fatal error occured: " << err.what() << ".\n";
    }
  catch (...)
    {
      std::cerr << "An unknown error was thrown.\n";
    }
  return EXIT_FAILURE;
}

#include "elle/Elle.hh"
#include "lune/Lune.hh"

static void _initAll()
{
  if (elle::Elle::Initialize() == elle::StatusError ||
      lune::Lune::Initialize() == elle::StatusError)
    throw std::runtime_error("Couldn't initialize !");
}

#include <elle/idiom/Close.hh>

#include <boost/filesystem.hpp>

#include <plasma/common/resources.hh>

#define NO_NETWORK_FILENAME ".dev_no_network"

static bool check_dirty_demo_hack()
{
  char const* infinit_home;

  if ((infinit_home = ::getenv("HOME")) == nullptr)
    {
      elle::log::warn("Couldn't find home directory");
      return false;
    }

  elle::log::info("Using infinit home =", infinit_home);
  namespace fs = boost::filesystem;

  auto no_network_file = (
    fs::path{infinit_home}
      .append(INFINIT_HOME_DIRECTORY, fs::path::codecvt())
      .append(NO_NETWORK_FILENAME, fs::path::codecvt())
  );

  if (!fs::exists(no_network_file))
    {
      elle::log::debug("demo not activated: no file", no_network_file.string());
      return false;
    }

  auto binary = fs::absolute(
    fs::path{infinit_home}
      .append(INFINIT_HOME_DIRECTORY, fs::path::codecvt())
      .append("bin/8infinit", fs::path::codecvt())
  );

  if (!fs::exists(binary))
    {
      elle::log::warn("Couldn't find infinit binary:", binary.string());
      return false;
    }

  auto mount_point = fs::absolute(
    fs::path{infinit_home}
      .append("Infinit_demo", fs::path::codecvt())
  );
  if (!fs::exists(mount_point))
    {
      elle::log::info("Creating demo mountpoint:", mount_point.string());
      fs::create_directories(mount_point);
    }

  auto cmd = binary.string() + " -n infinit_demo -m " + mount_point.string();

  elle::log::info("Starting infinit in rescue mode:", cmd);
  if (int res = ::system(cmd.c_str()) != 0)
      elle::log::fatal("Cannot launch infinit: exit code =", res);
  return true;
}
