#include "botcore_utils.hxx"
#include <cstdlib>
#include <cstring>
#include <stdio.h>
#include <string>

const char *Utils::GetEnv(const char *tag) noexcept
{
    const char *ret = std::getenv(tag);
    return ret ? ret : nullptr;
}

// runutil.cpp from samp source code (https://github.com/dashr9230/SA-MP/blob/main/announce/runutil.cpp)
//----------------------------------------------------

void RuntilH::Util_UrlUnencode(char *enc)
{
    char *write_pos = enc;

    while (*enc)
    {
        if (*enc == '%')
        {
            *write_pos = (*++enc > '/' && *enc < ':') ? ((*enc - ('0')) << 4) : ((*enc - ('7')) << 4);
            *write_pos |= (*++enc > '/' && *enc < ':') ? (*enc - '0') : (*enc - '7');
        }
        else if (*enc == '+')
            *write_pos = ' ';
        else
            *write_pos = *enc;

        write_pos++;
        enc++;
    }
    *write_pos = '\0';
}

//----------------------------------------------------

char RuntilH::Util_toupper(char c) { return ((c > (char)0x60) && (c < (char)0x7b)) ? c - 0x20 : c; }

//----------------------------------------------------

char *RuntilH::Util_stristr(const char *String, const char *Pattern)
{
    char *pptr, *sptr, *start;

    for (start = (char *)String; *start != 0; start++)
    {
        /* find start of pattern in string */
        for (; ((*start != 0) && (Util_toupper(*start) != Util_toupper(*Pattern))); start++)
            ;
        if (0 == *start)
            return NULL;

        pptr = (char *)Pattern;
        sptr = (char *)start;

        while (Util_toupper(*sptr) == Util_toupper(*pptr))
        {
            sptr++;
            pptr++;

            /* if end of pattern then pattern was found */

            if (0 == *pptr)
                return (start);
        }
    }
    return NULL;
}

//----------------------------------------------------

void RuntilH::Util_strupr(char *string)
{
    char *p = string;

    while (*p)
    {
        *p = Util_toupper(*p);
        p++;
    }
}

//----------------------------------------------------

int RuntilH::Util_wildcmp(char *wild, char *string)
{
    char *cp, *mp;

    while ((*string) && (*wild != '*'))
    {
        if ((*wild != *string) && (*wild != '?'))
        {
            return 0;
        }
        wild++;
        string++;
    }

    while (*string)
    {
        if (*wild == '*')
        {
            if (!*++wild)
            {
                return 1;
            }
            mp = wild;
            cp = string + 1;
        }
        else if ((*wild == *string) || (*wild == '?'))
        {
            wild++;
            string++;
        }
        else
        {
            wild = mp;
            string = cp++;
        }
    }

    while (*wild == '*')
    {
        wild++;
    }

    return !*wild;
}

//----------------------------------------------------

int RuntilH::Util_strnicmp(const char *s1, const char *s2, size_t n)
{

    if (n == 0)
        return 0;

    do
    {
        if (RuntilH::Util_toupper((unsigned char)*s1) != RuntilH::Util_toupper((unsigned char)*s2++))
            return (int)RuntilH::Util_toupper((unsigned char)*s1) - (int)RuntilH::Util_toupper((unsigned char)*--s2);
        if (*s1++ == 0)
            break;

    } while (--n != 0);

    return 0;
}

//----------------------------------------------------

char *RuntilH::Util_strrev(char *str)
{
    char *p1, *p2;

    if (!str || !*str)
        return str;
    for (p1 = str, p2 = str + strlen(str) - 1; p2 > p1; ++p1, --p2)
    {
        *p1 ^= *p2;
        *p2 ^= *p1;
        *p1 ^= *p2;
    }
    return str;
}

//----------------------------------------------------

char *RuntilH::Util_itoa(int v, char *s, int r)
{
    int i, neg = 0;
    char *p = s;
    char *q = s;

    if (r < 0 || r > 35)
    {
        *s = 0;
        return (s);
    }
    if (r == 0)
        r = 10;
    if (v == 0)
    {
        *p++ = '0';
        *p = 0;
        return (s);
    }
    if (v < 0)
    {
        neg = 1;
        v = -v;
    }
    while (v > 0)
    {
        i = v % r;
        if (i > 9)
            i += 7;
        *p++ = '0' + i;
        v /= r;
    }
    if (neg)
        *p++ = '-';
    *p-- = 0;
    q = s;
    while (p > q)
    {
        i = *q;
        *q++ = *p;
        *p-- = i;
    }
    return (s);
}

//----------------------------------------------------

void RuntilH::Util_Base64Encode(char *cpInput, char *cpOutput)
{
    int nIdx[4];
    while (*cpInput != '\0')
    {
        nIdx[0] = ((*cpInput) & 0xFC) >> 2;
        nIdx[1] = ((*cpInput) & 0x03) << 4;
        cpInput++;

        if (*cpInput != '\0')
        {
            nIdx[1] |= ((*cpInput) & 0xF0) >> 4;
            nIdx[2] = ((*cpInput) & 0x0F) << 2;
            cpInput++;

            if (*cpInput != '\0')
            {
                nIdx[2] |= ((*cpInput) & 0xC0) >> 6;
                nIdx[3] = (*cpInput) & 0x3F;
                cpInput++;
            }
            else
            {
                nIdx[3] = 64;
            }
        }
        else
        {
            nIdx[2] = 64;
            nIdx[3] = 64;
        }

        cpOutput[0] = BASE64_ENCOFING[nIdx[0]];
        cpOutput[1] = BASE64_ENCOFING[nIdx[1]];
        cpOutput[2] = BASE64_ENCOFING[nIdx[2]];
        cpOutput[3] = BASE64_ENCOFING[nIdx[3]];
        cpOutput += 4;
    }

    *cpOutput = '\0';
}

//----------------------------------------------------

int RuntilH::CanFileBeOpenedForReading(char *filename)
{
    FILE *f;
    if ((f = fopen(filename, "r")) != nullptr)
    {
        fclose(f);
        return 1;
    }
    return 0;
}

//----------------------------------------------------