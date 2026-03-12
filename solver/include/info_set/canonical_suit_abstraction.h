#include "info_set/info_set_abstraction.h"

class CanonicalSuitAbstraction : public InfoSetAbstraction {
  public:
      ShowdownKey GetShowdownKey(const Cards &community_cards) const override;

      PublicInfoKey GetPublicInfoKey(const Cards &community_cards,
                                     const std::vector<int> &history) const override;

      PrivateInfoKey GetPrivateInfoKey(const Cards &community_cards,
                                       const std::vector<Card> &hole_cards) const override;

      std::map<PrivateInfoKey, std::vector<Cards>> GetHandsByPrivateInfoKey(
          const Cards &community_cards) const override;
};
