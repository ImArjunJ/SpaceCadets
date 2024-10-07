#include <cpr/cpr.h>

#include <HtmlParser/Parser.hpp>
#include <format>
#include <iostream>

#include "HtmlParser/DOM.hpp"
#include "HtmlParser/Query.hpp"

// Reference: https://github.com/JustCabbage/HtmlParser/blob/master/src/Utilities.hpp
std::string Trim(const std::string& Input)
{
    const std::string Whitespace = " \t\n\r\f";
    const size_t Start = Input.find_first_not_of(Whitespace);
    if (Start == std::string::npos)
        return "";
    const size_t End = Input.find_last_not_of(Whitespace);
    return Input.substr(Start, End - Start + 1);
}

int main(int argc, char** argv)
{
    if (argc < 2)
    {
        std::cerr << "Usage: " << argv[0] << " <PersonId>" << std::endl;
        return 1;
    }

    std::string PersonId = argv[1];
    if (PersonId.find("@") != std::string::npos)
    {
        PersonId = PersonId.substr(0, PersonId.find("@"));
    }
    const cpr::Response r = cpr::Get(cpr::Url{std::format("https://www.ecs.soton.ac.uk/people/{}", PersonId)});

    if (r.status_code != 200)
    {
        std::cerr << "Failed to fetch the page" << std::endl;
        return 1;
    }

    HtmlParser::Parser Parser;
    Parser.SetStrict(false); // Disable strict mode to be more forgiving

    const HtmlParser::DOM Document = Parser.Parse(r.text);

    if (Document.Root() == nullptr)
    {
        std::cerr << "Failed to parse the page" << std::endl;
        return 1;
    }

    const HtmlParser::Query Query(Document.Root());

    const auto Name = Query.SelectFirst("h1.heading-m.inline-block.text-prussianDark");

    if (!Name)
    {
        std::cerr << "This user's profile does not seem to be public, or their email does not match the id" << std::endl;
        return 1;
    }

    const auto TitleNode = Query.SelectFirst("div.pb-6.text-xl");
    const auto AboutNode = Query.SelectFirst("section.sidetabs-section.mb-25");
    const auto TabBar = Query.SelectFirst("div[role='tablist']");

    const std::string Title = TitleNode == nullptr ? "Not Found" : TitleNode->GetTextContent();
    const std::string About = AboutNode == nullptr ? "Not Found" : AboutNode->GetTextContent();

    std::cout << "  Name: " << Name->GetTextContent() << "\n";
    std::cout << "  Title: " << Title << "\n";
    std::cout << "  About:\n";

    std::istringstream AboutStream(Trim(About));
    std::string Line;

    while (std::getline(AboutStream, Line))
    {
        std::cout << "    " << Line << "\n";
    }

    return 0;
}