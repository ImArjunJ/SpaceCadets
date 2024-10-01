#include "HtmlParser.hpp"

#include <format>

namespace HtmlParser
{
    std::string GetHtmlFromClass(const std::string& RawHtml, const std::string& ClassName, const std::string& HtmlElement)
    {
        const std::string OpeningTag = std::format("<{} class=\"{}\">", HtmlElement, ClassName);
        const std::string ClosingTag = std::format("</{}>", HtmlElement);

        const size_t OpeningTagPos = RawHtml.find(OpeningTag);
        if (OpeningTagPos == std::string::npos)
        {
            return "";
        }

        const size_t ClosingTagPos = RawHtml.find(ClosingTag, OpeningTagPos);
        if (ClosingTagPos == std::string::npos)
        {
            return "";
        }

        const size_t ContentStart = OpeningTagPos + OpeningTag.length();
        const size_t ContentLength = ClosingTagPos - ContentStart;

        return RawHtml.substr(ContentStart, ContentLength);
    }
} // namespace HtmlParser