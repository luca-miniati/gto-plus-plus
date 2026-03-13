#include "info_set/info_set_abstraction.h"

class U128Abstraction : public InfoSetAbstraction {
  public:
    PublicInfoKey GetPublicInfoKey(const GameState &state) const override;

    PrivateInfoKey GetPrivateInfoKey(const GameState &state,
        const std::vector<Card> &hole_cards) const override;
};
