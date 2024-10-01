#include <cpr/cpr.h>

#include <format>
#include <iostream>

#include "HtmlParser.hpp"

int main(int argc, char** argv)
{
    if (argc < 2)
    {
        std::cerr << "Usage: " << argv[0] << " <PersonId>" << std::endl;
        return 1;
    }

    // TODO: implement input sanitization
    std::string PersonId = argv[1];
    cpr::Response r = cpr::Get(cpr::Url{std::format("https://www.ecs.soton.ac.uk/people/{}", PersonId)});

    if (r.status_code != 200)
    {
        std::cerr << "Failed to fetch the page" << std::endl;
        return 1;
    }

    const std::string PersonName = HtmlParser::GetHtmlFromClass(r.text, "heading-m inline-block text-prussianDark", "h1");

    if (PersonName.empty())
    {
        std::cerr << "Person name not found" << std::endl;
        return 1;
    }

    std::cout << PersonName << std::endl;
}