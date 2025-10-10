#include "etl_test.h"
#include "etl_optional.h"
#include "etl_math.h"

namespace etl 
{
    bool test_all(Stream& trace)
    {
        trace.println("--- Embedded Template Library ---");
        auto result = test_optional(trace);
        trace.print("test_optional - ");    trace.println(result.isEmpty() ? "PASSED" : "FAILED: " + result);
        trace.println("--------------------------------");
        return true;
    }

    String test_optional(Stream& trace)
    {
        etl::optional<float> temperature;
        if(temperature) return "empty optinal";
        temperature = 36.6;
        if(!math::equals(temperature.value(), 36.6)) return "<float> assign and check value";
        if(math::equals(temperature.value(), 36.66)) return "<float> compare 36.66";

        etl::optional<int> hum(100);
        if(hum.has_value() && *hum != 100) return "<int> emplace assign and check value";
        hum.reset();
        if(hum.has_value()) return "reset failed";

        int value1 = hum.value_or(5); 
        if(hum.has_value() || value1 != 5) return "value_or(5) failed";

        return "";  // no errors
    }
}