#include "NF.h"

namespace nf
{
    /**
     * @param nf    The NF to convert
     * @return A string representation of the input NF
     */
    std::string nf_to_string(NF nf) {
        switch (nf) {
            case snort:
                return "snort";

            case haproxy:
                return "haproxy";
        }

    }
}