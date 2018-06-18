
#include <iostream>

#include <boost/predef.h>

#include <boost/program_options/options_description.hpp>
#include <boost/program_options/parsers.hpp>
#include <boost/program_options/variables_map.hpp>

#include "android_capture.hpp"

//------------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    std::string dest_path = "";
    std::string dest_fourcc = "";
    bool verbose = false;
    unsigned camera_id = 0;

    try
    {
        namespace po = boost::program_options;

        po::options_description opt_description;
        opt_description.add_options()
                ("help,h", "show help")
                ("destination,d", po::value<std::string>(&dest_path)->required(), "output file")
                ("output_codec,e", po::value<std::string>(&dest_fourcc)->default_value("X264"), "output file codec")
                ("verbose,v", po::bool_switch(&verbose)->default_value(false), "enable verbose output")
                ("camera_id,c", po::value<unsigned>(&camera_id), "camera id to be used");

        po::variables_map vm;
        po::store(po::parse_command_line(argc, argv, opt_description), vm);
        po::notify(vm);
    }
    catch(std::exception& ex) {
        std::cout << "Unable to parse command line options " << ex.what() << std::endl;
        return -1;
    }

    try
    {
        run_capture(dest_path, dest_fourcc);
    }
    catch (const std::exception &e)
    {
        //log << "Exception: " << e.what() << endl;
        std::cout << "Exception: " << e.what() << std::endl;
        return -1;
    }

    return 0;
}
