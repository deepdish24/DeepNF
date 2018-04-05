#include "Field.h"

namespace field
{
    /**
     * @param nf    The Field to convert
     * @return A string representation of the input Field
     */
    std::string field_to_string(Field field) {
        switch (field) {
            case SIP:
                return "SIP";

            case DIP:
                return "DIP";

            case SPORT:
                return "SPORT";

            case DPORT:
                return "DPORT";

            case PAYLOAD:
                return "PAYLOAD";
        }
    }
}