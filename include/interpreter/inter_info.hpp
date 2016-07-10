#ifndef INTER_INFO
#define INTER_INFO

#include <cinttypes>
#include <cstring>
#include <string>

class inter_info_t
{
public:
    inter_info_t()
        : alphas(0), betas(0), etas(0), taus(0), nodes(0)
    {
        this->clear();
    }

    inter_info_t(uint64_t alpha, uint64_t beta, uint64_t eta, uint64_t tau, uint64_t node)
        : alphas(alpha), betas(beta), etas(eta), taus(tau), nodes(node) { }

    inline  void clear()
    {
        std::memset(this, 0, sizeof(*this));
    }

    inter_info_t& operator+=(inter_info_t const& other)
    {
        this->alphas += other.alphas;
        this->betas  += other.betas;
        this->etas   += other.etas;
        this->taus   += other.taus;
        this->nodes  += other.nodes;

        return *this;
    }

    std::wstring to_str() const
    {
        return L"Alphas: " +std::to_wstring(alphas) + L" Betas: " +std::to_wstring(betas)
              +L" Etas: "  +std::to_wstring(etas  ) + L" Taus: "  +std::to_wstring(taus)
              +L" Nodes: " +std::to_wstring(nodes );
    }

    uint64_t alphas, betas, etas, taus, nodes;
};

#endif // INTER_INFO
