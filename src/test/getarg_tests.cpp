#include <boost/algorithm/string.hpp>
#include <boost/foreach.hpp>
#include <boost/test/unit_test.hpp>

#include "util.h"

BOOST_AUTO_TEST_SUITE(getarg_tests)

static void
ResetArgs(const std::string& strArg)
{
    std::vector<std::string> vecArg;
    boost::split(vecArg, strArg, boost::is_space(), boost::token_compress_on);

    // Insert dummy executable name:
    vecArg.insert(vecArg.begin(), "testbitcoin");

    // Convert to char*:
    std::vector<const char*> vecChar;
    BOOST_FOREACH(std::string& s, vecArg)
        vecChar.push_back(s.c_str());

    ParseParameters(vecChar.size(), &vecChar[0]);
}

BOOST_AUTO_TEST_CASE(boolarg)
{
    ResetArgs("-XTN");
    BOOST_CHECK(GetBoolArg("-XTN"));
    BOOST_CHECK(GetBoolArg("-XTN", false));
    BOOST_CHECK(GetBoolArg("-XTN", true));

    BOOST_CHECK(!GetBoolArg("-fo"));
    BOOST_CHECK(!GetBoolArg("-fo", false));
    BOOST_CHECK(GetBoolArg("-fo", true));

    BOOST_CHECK(!GetBoolArg("-XTNo"));
    BOOST_CHECK(!GetBoolArg("-XTNo", false));
    BOOST_CHECK(GetBoolArg("-XTNo", true));

    ResetArgs("-XTN=0");
    BOOST_CHECK(!GetBoolArg("-XTN"));
    BOOST_CHECK(!GetBoolArg("-XTN", false));
    BOOST_CHECK(!GetBoolArg("-XTN", true));

    ResetArgs("-XTN=1");
    BOOST_CHECK(GetBoolArg("-XTN"));
    BOOST_CHECK(GetBoolArg("-XTN", false));
    BOOST_CHECK(GetBoolArg("-XTN", true));

    // New 0.6 feature: auto-map -nosomething to !-something:
    ResetArgs("-noXTN");
    BOOST_CHECK(!GetBoolArg("-XTN"));
    BOOST_CHECK(!GetBoolArg("-XTN", false));
    BOOST_CHECK(!GetBoolArg("-XTN", true));

    ResetArgs("-noXTN=1");
    BOOST_CHECK(!GetBoolArg("-XTN"));
    BOOST_CHECK(!GetBoolArg("-XTN", false));
    BOOST_CHECK(!GetBoolArg("-XTN", true));

    ResetArgs("-XTN -noXTN");  // -XTN should win
    BOOST_CHECK(GetBoolArg("-XTN"));
    BOOST_CHECK(GetBoolArg("-XTN", false));
    BOOST_CHECK(GetBoolArg("-XTN", true));

    ResetArgs("-XTN=1 -noXTN=1");  // -XTN should win
    BOOST_CHECK(GetBoolArg("-XTN"));
    BOOST_CHECK(GetBoolArg("-XTN", false));
    BOOST_CHECK(GetBoolArg("-XTN", true));

    ResetArgs("-XTN=0 -noXTN=0");  // -XTN should win
    BOOST_CHECK(!GetBoolArg("-XTN"));
    BOOST_CHECK(!GetBoolArg("-XTN", false));
    BOOST_CHECK(!GetBoolArg("-XTN", true));

    // New 0.6 feature: treat -- same as -:
    ResetArgs("--XTN=1");
    BOOST_CHECK(GetBoolArg("-XTN"));
    BOOST_CHECK(GetBoolArg("-XTN", false));
    BOOST_CHECK(GetBoolArg("-XTN", true));

    ResetArgs("--noXTN=1");
    BOOST_CHECK(!GetBoolArg("-XTN"));
    BOOST_CHECK(!GetBoolArg("-XTN", false));
    BOOST_CHECK(!GetBoolArg("-XTN", true));

}

BOOST_AUTO_TEST_CASE(stringarg)
{
    ResetArgs("");
    BOOST_CHECK_EQUAL(GetArg("-XTN", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-XTN", "eleven"), "eleven");

    ResetArgs("-XTN -bar");
    BOOST_CHECK_EQUAL(GetArg("-XTN", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-XTN", "eleven"), "");

    ResetArgs("-XTN=");
    BOOST_CHECK_EQUAL(GetArg("-XTN", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-XTN", "eleven"), "");

    ResetArgs("-XTN=11");
    BOOST_CHECK_EQUAL(GetArg("-XTN", ""), "11");
    BOOST_CHECK_EQUAL(GetArg("-XTN", "eleven"), "11");

    ResetArgs("-XTN=eleven");
    BOOST_CHECK_EQUAL(GetArg("-XTN", ""), "eleven");
    BOOST_CHECK_EQUAL(GetArg("-XTN", "eleven"), "eleven");

}

BOOST_AUTO_TEST_CASE(intarg)
{
    ResetArgs("");
    BOOST_CHECK_EQUAL(GetArg("-XTN", 11), 11);
    BOOST_CHECK_EQUAL(GetArg("-XTN", 0), 0);

    ResetArgs("-XTN -bar");
    BOOST_CHECK_EQUAL(GetArg("-XTN", 11), 0);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 0);

    ResetArgs("-XTN=11 -bar=12");
    BOOST_CHECK_EQUAL(GetArg("-XTN", 0), 11);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 12);

    ResetArgs("-XTN=NaN -bar=NotANumber");
    BOOST_CHECK_EQUAL(GetArg("-XTN", 1), 0);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 0);
}

BOOST_AUTO_TEST_CASE(doubledash)
{
    ResetArgs("--XTN");
    BOOST_CHECK_EQUAL(GetBoolArg("-XTN"), true);

    ResetArgs("--XTN=verbose --bar=1");
    BOOST_CHECK_EQUAL(GetArg("-XTN", ""), "verbose");
    BOOST_CHECK_EQUAL(GetArg("-bar", 0), 1);
}

BOOST_AUTO_TEST_CASE(boolargno)
{
    ResetArgs("-noXTN");
    BOOST_CHECK(!GetBoolArg("-XTN"));
    BOOST_CHECK(!GetBoolArg("-XTN", true));
    BOOST_CHECK(!GetBoolArg("-XTN", false));

    ResetArgs("-noXTN=1");
    BOOST_CHECK(!GetBoolArg("-XTN"));
    BOOST_CHECK(!GetBoolArg("-XTN", true));
    BOOST_CHECK(!GetBoolArg("-XTN", false));

    ResetArgs("-noXTN=0");
    BOOST_CHECK(GetBoolArg("-XTN"));
    BOOST_CHECK(GetBoolArg("-XTN", true));
    BOOST_CHECK(GetBoolArg("-XTN", false));

    ResetArgs("-XTN --noXTN");
    BOOST_CHECK(GetBoolArg("-XTN"));

    ResetArgs("-noXTN -XTN"); // XTN always wins:
    BOOST_CHECK(GetBoolArg("-XTN"));
}

BOOST_AUTO_TEST_SUITE_END()
