#pragma once
#include <string>

namespace HtmlParser
{
    std::string GetHtmlFromClass(const std::string& RawHtml, const std::string& ClassName, const std::string& HtmlElement = "div");
}