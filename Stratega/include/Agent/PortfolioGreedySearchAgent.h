#pragma once
#include <Agent/Agent.h>
#include <Agent/TreeSearchAgents/TreeNode.h>
#include <Agent/Heuristic/SimpleHeuristic.h>
#include <Agent/Heuristic/LinearSumHeuristic.h>

#include <Agent/Portfolios/BasePortfolio.h>
#include <Agent/Portfolios/AttackClosest.h>
#include <Agent/Portfolios/AttackWeakest.h>
#include <Agent/Portfolios/RunAway.h>
#include <Agent/Portfolios/UseSpecialAbility.h>
#include <Agent/Portfolios/RunToFriends.h>
#include <Agent/Portfolios/RandomPortfolio.h>

namespace SGA
{
	struct PortfolioGreedySearchParams {
		// agent parameters
		int MAX_FM_CALLS = 2000;				// the maximum number of forward model calls (can be slightly exceeded in case the next generation takes more evaluations)
		int REMAINING_FM_CALLS = MAX_FM_CALLS;	// the number of remaining forward model calls
		int PLAYER_ID = -1;						// the agents ID in the current game
		int NR_OF_TURNS_PLANNED = 2;			// the agents ID in the current game
		int ITERATIONS_PER_IMPROVE = 5;			// the number of iterations for one call of the improve method
		
		std::vector<std::unique_ptr<BasePortfolio>> portfolios = std::vector<std::unique_ptr<BasePortfolio>>();	// the portfolios used to sample actions of a genome
		LinearSumHeuristic HEURISTIC{ LinearSumHeuristic() };	// the heuristic used to evaluate a genome
		PortfolioGreedySearchParams()
		{
			std::unique_ptr<BasePortfolio> attackClose = std::make_unique<AttackClosest>();
			portfolios.emplace_back(std::move(attackClose));
			std::unique_ptr<BasePortfolio> attackWeak = std::make_unique<AttackWeakest>();
			portfolios.emplace_back(std::move(attackWeak));
			std::unique_ptr<BasePortfolio> runAway = std::make_unique<RunAway>();
			portfolios.emplace_back(std::move(runAway));
			std::unique_ptr<BasePortfolio> useSpecialAbility = std::make_unique<UseSpecialAbility>();
			portfolios.emplace_back(std::move(useSpecialAbility));
			std::unique_ptr<BasePortfolio> runToFriends = std::make_unique<RunToFriends>();
			portfolios.emplace_back(std::move(runToFriends));
			
			std::unique_ptr<BasePortfolio> random = std::make_unique<RandomPortfolio>();
			portfolios.emplace_back(std::move(random));
		}
	};
		
	class PortfolioGreedySearchAgent : public Agent
	{
	private:
		PortfolioGreedySearchParams params_;
		
	public:
		PortfolioGreedySearchAgent() :
			Agent{ }, params_(PortfolioGreedySearchParams())
		{}

		void runTBS(TBSGameCommunicator& gameCommunicator, TBSForwardModel forwardModel) override;

	private:
		void Improve(TBSForwardModel& forwardModel, TBSGameState& gameState, std::vector<TBSUnit*>& playerUnits, std::map<int, BasePortfolio*>& playerPortfolios, std::map<int, BasePortfolio*>& opponentPortfolios);

		double Playout(TBSForwardModel& forwardModel, TBSGameState gameState, std::map<int, BasePortfolio*>& playerPortfolios, std::map<int, BasePortfolio*>& opponentPortfolios);

		void InitializePortfolios(std::vector<TBSUnit*>& units, std::map<int, BasePortfolio*>& portfolioMap);
		
		static Action<Vector2i> GetPortfolioAction(TBSGameState& gameState, std::unique_ptr<ActionSpace<Vector2i>>& actionSpace, std::map<int, BasePortfolio*>& portfolioMap1, std::map<int, BasePortfolio*>& portfolioMap2);

		void applyActionToGameState(const TBSForwardModel& forwardModel, TBSGameState& gameState, std::unique_ptr<ActionSpace<Vector2i>>& actionSpace, const Action<Vector2i>& action);
	};
}
