#include <vector>
#include <iostream>
#include <fstream>
#include <stdexcept>
#include <sstream>
#include <gzip/compress.hpp>

int main(int argc, char** argv) {
  std::vector<std::string> args;
  bool verbose = false;
  for (int i=1;i<argc;++i)
  {
      if (strcmp(argv[i], "--verbose") == 0) {
          verbose = true;
      } else {
          args.push_back(argv[i]);
      }
  }

  if (args.empty())
  {
      std::clog << "please pass the path to an uncompressed, zlib-compressed, or gzip compressed protobuf tile\n";
      return -1;
  }

  try
  {
      std::string filename = args[0];
      std::ifstream stream(filename.c_str(),std::ios_base::in|std::ios_base::binary);
      if (!stream.is_open())
      {
          throw std::runtime_error("could not open: '" + filename + "'");
      }

      // we are using lite library, so we copy to a string instead of using ParseFromIstream
      std::string message(std::istreambuf_iterator<char>(stream.rdbuf()),(std::istreambuf_iterator<char>()));
      stream.close();
      std::string value = gzip::compress(message.data(), message.size());

  }
  catch (std::exception const& ex)
  {
      std::clog << "error: " << ex.what() << "\n";
      return -1;
  }


}
