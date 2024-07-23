#ifndef XC_HPP
#define XC_HPP

#include <string>

namespace xc
{
    enum colour
    {
        NONE = 0,
        BLACK,
        RED,
        GREEN,
        YELLOW,
        BLUE,
        MAGENTA,
        CYAN,
        WHITE,
        BBLACK,
        BRED,
        BGREEN,
        BYELLOW,
        BBLUE,
        BMAGENTA,
        BCYAN,
        BWHITE
    };

    enum style
    {
        UNDEFINED = 0,
        BOLD = 1,
        UNDERLINE = 4,
        BLINKING = 5,
        REVERSED = 7,
        CONCEALED = 8
    };

    static std::string get_style(const style& s) { return !s ? "" : std::to_string(s); }
    static std::string get_colour(const colour& c, bool background) { return !c ? "" : std::to_string(c + (c >= BBLACK ? 81 : 29) + (background ? 10 : 0)); }

    static std::string set(const colour& foreground, const colour& background, const style& s, const std::string& str)
    {
        std::string tokens[5] = 
        { 
            "\033[", 
            get_style(s), 
            get_colour(foreground, false),
            get_colour(background, true),
            str.empty() ? "" : "\033[0m"
        };
        int count = !tokens[1].empty() + !tokens[2].empty() + !tokens[3].empty();
        
        if (count <= 1)
            return tokens[0] + tokens[1] + tokens[2] + tokens[3] + "m" + str + tokens[4];
        
        else if (count == 2)
            return tokens[1].empty() ? 
                tokens[0] + tokens[1] + tokens[2] + ";" + tokens[3] + "m" + str + tokens[4] :
                tokens[0] + tokens[1] + ";" + tokens[2] + tokens[3] + "m" + str + tokens[4];				
        
        else if (count == 3)
            return tokens[0] + tokens[1] + ";" + tokens[2] + ";" + tokens[3] + "m" + str + tokens[4];

        return str;
    }

    static std::string set(const colour& foreground, const colour& background, const style& s) { return set(foreground, background, s, ""); }
    static std::string set(const colour& foreground, const colour& background, const std::string& str) { return set(foreground, background, style::UNDEFINED, str); }
    static std::string set(const colour& foreground, const colour& background) { return set(foreground, background, style::UNDEFINED, ""); }
    static std::string set(const colour& foreground, const style& s, const std::string& str) { return set(foreground, colour::NONE, s, str); }
    static std::string set(const colour& foreground, const style& s) { return set(foreground, colour::NONE, s, ""); }

    static std::string reset() { return "\033[0m"; }

    static std::string red(const std::string& str = "") { return set(colour::RED, colour::NONE, style::UNDEFINED, str); }
    static std::string green(const std::string& str = "") { return set(colour::GREEN, colour::NONE, style::UNDEFINED, str); }
    static std::string blue(const std::string& str = "") { return set(colour::BLUE, colour::NONE, style::UNDEFINED, str); }
    static std::string yellow(const std::string& str = "") { return set(colour::YELLOW, colour::NONE, style::UNDEFINED, str); }
    static std::string magenta(const std::string& str = "") { return set(colour::MAGENTA, colour::NONE, style::UNDEFINED, str); }
    static std::string cyan(const std::string& str = "") { return set(colour::CYAN, colour::NONE, style::UNDEFINED, str); }
    static std::string black(const std::string& str = "") { return set(colour::BLACK, colour::NONE, style::UNDEFINED, str); }
    static std::string white(const std::string& str = "") { return set(colour::WHITE, colour::NONE, style::UNDEFINED, str); }

    static std::string bred(const std::string& str = "") { return set(colour::BRED, colour::NONE, style::UNDEFINED, str); }
    static std::string bgreen(const std::string& str = "") { return set(colour::BGREEN, colour::NONE, style::UNDEFINED, str); }
    static std::string bblue(const std::string& str = "") { return set(colour::BBLUE, colour::NONE, style::UNDEFINED, str); }
    static std::string byellow(const std::string& str = "") { return set(colour::BYELLOW, colour::NONE, style::UNDEFINED, str); }
    static std::string bmagenta(const std::string& str = "") { return set(colour::BMAGENTA, colour::NONE, style::UNDEFINED, str); }
    static std::string bcyan(const std::string& str = "") { return set(colour::BCYAN, colour::NONE, style::UNDEFINED, str); }
    static std::string bblack(const std::string& str = "") { return set(colour::BBLACK, colour::NONE, style::UNDEFINED, str); }
    static std::string bwhite(const std::string& str = "") { return set(colour::BWHITE, colour::NONE, style::UNDEFINED, str); }

    static std::string bold(const std::string& str = "") { return set(colour::NONE, colour::NONE, style::BOLD, str); }
    static std::string underline(const std::string& str = "") { return set(colour::NONE, colour::NONE, style::UNDERLINE, str); }
    static std::string blinking(const std::string& str = "") { return set(colour::NONE, colour::NONE, style::BLINKING, str); }
    static std::string reversed(const std::string& str = "") { return set(colour::NONE, colour::NONE, style::REVERSED, str); }
    static std::string concealed(const std::string& str = "") { return set(colour::NONE, colour::NONE, style::CONCEALED, str); }

    namespace bg
    {
        static std::string red(const std::string& str = "") { return set(colour::NONE, colour::RED, style::UNDEFINED, str); }
        static std::string green(const std::string& str = "") { return set(colour::NONE, colour::GREEN, style::UNDEFINED, str); }
        static std::string blue(const std::string& str = "") { return set(colour::NONE, colour::BLUE, style::UNDEFINED, str); }
        static std::string yellow(const std::string& str = "") { return set(colour::NONE, colour::YELLOW, style::UNDEFINED, str); }
        static std::string magenta(const std::string& str = "") { return set(colour::NONE, colour::MAGENTA, style::UNDEFINED, str); }
        static std::string cyan(const std::string& str = "") { return set(colour::NONE, colour::CYAN, style::UNDEFINED, str); }
        static std::string black(const std::string& str = "") { return set(colour::NONE, colour::BLACK, style::UNDEFINED, str); }
        static std::string white(const std::string& str = "") { return set(colour::NONE, colour::WHITE, style::UNDEFINED, str); }

        static std::string bred(const std::string& str = "") { return set(colour::NONE, colour::BRED, style::UNDEFINED, str); }
        static std::string bgreen(const std::string& str = "") { return set(colour::NONE, colour::BGREEN, style::UNDEFINED, str); }
        static std::string bblue(const std::string& str = "") { return set(colour::NONE, colour::BBLUE, style::UNDEFINED, str); }
        static std::string byellow(const std::string& str = "") { return set(colour::NONE, colour::BYELLOW, style::UNDEFINED, str); }
        static std::string bmagenta(const std::string& str = "") { return set(colour::NONE, colour::BMAGENTA, style::UNDEFINED, str); }
        static std::string bcyan(const std::string& str = "") { return set(colour::NONE, colour::BCYAN, style::UNDEFINED, str); }
        static std::string bblack(const std::string& str = "") { return set(colour::NONE, colour::BBLACK, style::UNDEFINED, str); }
        static std::string bwhite(const std::string& str = "") { return set(colour::NONE, colour::BWHITE, style::UNDEFINED, str); }
    }
}


#endif // XC_HPP