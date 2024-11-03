#include "libstat.h"

#include <cassert>
#include <cmath>
#include <sstream>

namespace statistics::tests {

    void AggregSum() {
        aggregations::Sum aggreg;
        assert(*aggreg.Get() == 0);

        aggreg.PutValue(10.);
        aggreg.PutValue(20.);
        aggreg.PutValue(-40.);

        assert(*aggreg.Get() == -10.);
    }

    void AggregMax() {
        aggregations::Max aggreg;
        assert(!aggreg.Get());

        aggreg.PutValue(10.);
        aggreg.PutValue(20.);
        aggreg.PutValue(-40.);

        assert(*aggreg.Get() == 20.);
    }

    void AggregMean() {
        aggregations::Mean aggreg;
        assert(!aggreg.Get());

        aggreg.PutValue(10.);
        aggreg.PutValue(20.);
        aggreg.PutValue(-40.);
        aggreg.PutValue(30.);

        assert(*aggreg.Get() == 5.);
    }

    void AggregStandardDeviation() {
        aggregations::StandardDeviation aggreg;
        assert(!aggreg.Get());

        aggreg.PutValue(10.);
        aggreg.PutValue(10.);
        aggreg.PutValue(10.);
        aggreg.PutValue(10.);

        assert(std::abs(*aggreg.Get()) < 1e-5);

        aggreg.PutValue(20.);
        aggreg.PutValue(20.);
        aggreg.PutValue(20.);
        aggreg.PutValue(20.);

        assert(std::abs(*aggreg.Get() - 5.) < 1e-5);
    }

    void AggregMode() {
        aggregations::Mode aggreg;
        assert(!aggreg.Get());

        aggreg.PutValue(1.1);
        aggreg.PutValue(0.9);
        aggreg.PutValue(2.1);
        aggreg.PutValue(2.2);
        aggreg.PutValue(2.1);
        aggreg.PutValue(-1.0);
        aggreg.PutValue(3.0);
        aggreg.PutValue(3.0);
        aggreg.PutValue(1000.);

        assert(std::round(*aggreg.Get()) == 2.);
    }

    void AggregPrinter() {
        statistics::AggregPrinter<aggregations::Max> printer_instance;

        assert(detail::GetPrinterValue(printer_instance) == "max is undefined\n");
        printer_instance.PutValue(10.);
        printer_instance.PutValue(20.);
        printer_instance.PutValue(-40.);

        std::ostringstream out;
        out << 20.;

        assert(detail::GetPrinterValue(printer_instance) == "max is " + out.str() + "\n");
    }

} // namespace statistics::tests