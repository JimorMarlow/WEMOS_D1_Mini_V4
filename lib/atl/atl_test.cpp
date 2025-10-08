#include "atl_test.h"
#include "atl_optional.h"

namespace atl 
{
    bool test_all(Stream& trace)
    {
        trace.println("--- Arduino Template Library ---");
        auto result = test_optional();
        trace.print("test_optional - ");    trace.println(result.isEmpty() ? "PASSED" : "FAILED: " + result);
        trace.println("--------------------------------");
        return true;
    }

    String test_optional()
    {
        atl::optional<float> temperature;
        if(temperature) return "empty optinal";
        temperature = 36.6;
        if(*temperature != 36.6) return "<float> assign and check value";

        atl::optional<int> hum(100);
        if(hum.hasValue() && *hum != 100) return "<int> emplace assign and check value";

        return "";  // no errors
    }
}