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

std::string ToLower(const std::string& Input)
{
    std::string Result = Input;
    std::transform(Result.begin(), Result.end(), Result.begin(), [](unsigned char c) { return std::tolower(c); });
    return Result;
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
    cpr::Response r = cpr::Get(cpr::Url{std::format("https://www.ecs.soton.ac.uk/people/{}", PersonId)});

    if (r.status_code != 200)
    {
        std::cerr << "Failed to fetch the page" << std::endl;
        return 1;
    }

    HtmlParser::Parser Parser;
    Parser.SetStrict(false); // Disable strict mode to be more forgiving

    HtmlParser::DOM Document = Parser.Parse(r.text);

    if (Document.Root() == nullptr)
    {
        std::cerr << "Failed to parse the page" << std::endl;
        return 1;
    }

    HtmlParser::Query Query(Document.Root());

    auto Name = Query.SelectFirst("h1.heading-m.inline-block.text-prussianDark");

    if (!Name)
    {
        // try and find /people/{} code from email if not found initially
        r = cpr::Get(cpr::Url{"https://www.ecs.soton.ac.uk/people/placeholder"});

        if (r.status_code != 200)
        {
            std::cerr << "Failed to fetch the page" << std::endl;
            return 1;
        }

        Document = Parser.Parse(r.text);
        Query = HtmlParser::Query(Document.Root());
        auto List = Query.SelectFirst("tbody.list");
        if (!List)
        {
            std::cerr << "Failed to find the list" << std::endl;
            return 1;
        }

        for (const auto& Row : List->Children)
        {
            if (Row->Children.size() != 4)
                continue;

            const auto Email = ToLower(Row->Children[3]->GetTextContent());
            if (Email.empty())
                continue;

            if (Email.find(ToLower(argv[1])) != std::string::npos || Email.find(ToLower(PersonId)) != std::string::npos)
            {
                /*ssss
                <td>
                <span class="js-tableSort-name" style="display:none">Rogers,Eric</span>
                <a href="/people/er">Professor Eric Rogers</a>
                </td>
                */
                const auto EmailTdNode = Row->Children[0];
                if (EmailTdNode->Children.size() != 2)
                    continue;
                const auto Link = EmailTdNode->Children[1]->GetAttribute("href");
                if (Link.empty())
                    continue;

                PersonId = Link.substr(8);
                r = cpr::Get(cpr::Url{std::format("https://www.ecs.soton.ac.uk/people/{}", PersonId)});

                if (r.status_code != 200)
                {
                    std::cerr << "Failed to fetch the page" << std::endl;
                    return 1;
                }

                Document = Parser.Parse(r.text);

                if (Document.Root() == nullptr)
                {
                    std::cerr << "Failed to parse the page" << std::endl;
                    return 1;
                }

                Query = HtmlParser::Query(Document.Root());
                Name = Query.SelectFirst("h1.heading-m.inline-block.text-prussianDark");
                break;
            }
        }

        if (!Name)
        {
            std::cerr << "Failed to find the person" << std::endl;
            return 1;
        }
    }

    const auto TitleNode = Query.SelectFirst("div.pb-6.text-xl");
    auto AboutNode = Query.SelectFirst("section.sidetabs-section.mb-25");
    const auto TabBar = Query.SelectFirst("div[role='tablist']");

    if (Query.SelectFirst("section#about") == nullptr)
        AboutNode = nullptr;

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