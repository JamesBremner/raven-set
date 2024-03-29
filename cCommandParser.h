#include <iostream>
#include <sstream>
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
         *
         * Usage:
         *
<pre>
int main(int argc, char *argv[])
{
    raven::set::cCommandParser P;
    P.add("help", "\tproduce help message");
    P.add("solve", "fname\tsolve a maze");
    P.add("gen", "bin|rec\tgenerate maze with binary or recursive algorithm");
    P.add("rows", "count\trows in generated maze");
    P.add("cols", "count\tcolumns in generated maze");
    P.add("path", "prioritize critical path tasks ( default: off )","bool");
    P.parse(argc, argv);

    auto fname = P.value("solve");
    myTaskGraph.flagCritPath = ( P.value("path") == "t");
    ...
</pre>
         */
        class cCommandParser
        {
            enum class eType
            {
                value,
                yesno
            };

            /// private class for a command line option, used only by cCommandParser
            class cCommandOption
            {
            public:
                cCommandOption(
                    const std::string &name,
                    const std::string &desc,
                    eType type = eType::value)
                    : myName(name),
                      myDesc(desc),
                      myType(type)
                {
                    if (myType == eType::yesno)
                        myValue = "f";
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
                eType type() const
                {
                    return myType;
                }

            private:
                std::string myName;
                std::string myDesc;
                std::string myValue;
                eType myType;
            };

        public:
            /** Add an option to be parsed
             * @param[in] name of the option, used for prompt and for retrieving the value
             * @param[in] desc of the option, for the help display
             * @param[in] type defaults to value, "bool" for boolean
             * 
             * A value type expects "--name <value>" in the command line
             * A boolean type expects "--name" in the command line.  It sets value to "t" if present, otherwise "f"
             */
            void add(
                const std::string &name,
                const std::string &desc,
                const std::string &type = "")
            {
                if (type.size())
                    myOption.insert(
                        std::make_pair(
                            name,
                            cCommandOption(name, desc, eType::yesno)));
                else
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
                //for (int k = 1; k < ac; k++)
                while( iss.good() )
                {
                    iss >> token;
                    if (token == "--help")
                    {
                        describe();
                        exit(0);
                    }
                    auto o = myOption.find(token.substr(2));
                    if (o == myOption.end())
                        continue;
                    if (o->second.type() == eType::value) {
                        // read command value
                        iss >> v;
                    }
                    else
                        v = "t";
                    o->second.value(v);
                }
            }
            /// get the option value by name, must be called after parse()
            /// @return value ("t" or "f" for boolean type )
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
