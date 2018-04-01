#ifndef DEEPNF_NF_H
#define DEEPNF_NF_H

enum NF {
    snort,
    haproxy,
    end // used only for iterating over the NF enums; not an actual NF
};

namespace nf
{
    /**
     * @param nf    The NF to convert
     * @return A string representation of the input NF
     */
    std::string nf_to_string(NF nf);
}
#endif