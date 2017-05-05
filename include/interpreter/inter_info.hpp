#ifndef INTER_INFO
#define INTER_INFO

#include <cinttypes>
#include <cstring>
#include <string>

class inter_info_t
{
public:
	inter_info_t()
		: betas(0), etas(0), steps(0), nodes(0)
	{ }

	inter_info_t(uint64_t beta, uint64_t eta, uint64_t steps, uint64_t node)
		: betas(beta), etas(eta), steps(steps), nodes(node)
	{ }

	inline void clear()
	{
		std::memset(this, 0, sizeof(*this));
	}

	inter_info_t& operator +=(inter_info_t const& other)
	{
		this->betas  += other.betas;
		this->etas   += other.etas;
		this->steps  += other.steps;
		this->nodes  += other.nodes;

		return *this;
	}

	std::wstring to_str() const
	{
		return L" Betas: " +std::to_wstring(betas)
			  +L" Etas: "  +std::to_wstring(etas ) + L" Steps: "  +std::to_wstring(steps)
			  +L" Nodes: " +std::to_wstring(nodes);
	}

	std::uint64_t betas, etas, steps, nodes;
};

#endif // INTER_INFO
