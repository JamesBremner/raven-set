#include <iostream>
#include <iomanip>
#include <map>
namespace raven
{
    namespace set
    {
        /** A command line option parser
         *
         * This implements the main features of boost::program_options
         * but using a tiny amount of standard C++17 code in a self contained header
         */
        class cCommandParser
        {
            /// private class for a command line option, used only by cCommandParser
            class cCommandOption
            {
            public:
                cCommandOption(
                    const std::string &name,
                    const std::string &desc)
                    : myName(name),
                      myDesc(desc)
                {
                }
                void describe() const
                {
                    std::cout << " --"
                              << std::setw(10) << std::left << myName
                              << "\t" << myDesc << "\n";
                }
                void value(const std::string &v)
                {
                    myValue = v;
                }
                std::string value() const
                {
                    return myValue;
                }

            private:
                std::string myName;
                std::string myDesc;
                std::string myValue;
            };

        public:
            /** Add an option to be parsed
             * @param[in] name of the option, used for prompt and for retrieving the value
             * @param[in] desc of the option, for the help display
             */
            void add(
                const std::string &name,
                const std::string &desc)
            {
                myOption.insert(
                    std::make_pair(
                        name,
                        cCommandOption(name, desc)));
            }
            /// Display the help message describing the options on the console
            void describe()
            {
                std::cout << "\n";
                for (auto o : myOption)
                    o.second.describe();
                std::cout << "\n";
            }
            /** Parse the command line
             * @param[in] ac argument count, including the executable name
             * @param[in] av the arguents in an array of character strings
             *
             * Must be called after all options have been specified using add()
             */
            void parse(int ac, char **av)
            {
                if (ac <= 1)
                {
                    describe();
                    exit(0);
                }

                std::string myLine;
                for (int t = 1; t < ac; t++)
                    myLine += std::string(av[t]) + " ";

                std::istringstream iss(myLine);
                std::string token, v;
                for (int k = 0; k < (ac - 1) / 2; k++)
                {
                    iss >> token >> v;
                    if (token == "--help")
                    {
                        describe();
                        exit(0);
                    }
                    auto o = myOption.find(token.substr(2));
                    if (o == myOption.end())
                        continue;
                    o->second.value(v);
                }
            }
            /// get the option value by name, must be called after parse()
            std::string value(const std::string name) const
            {
                auto o = myOption.find(name);
                if (o == myOption.end())
                    return "";
                return o->second.value();
            }

        private:
            std::map<std::string, cCommandOption> myOption;
        };
    }
}
