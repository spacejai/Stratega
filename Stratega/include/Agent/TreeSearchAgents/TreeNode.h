#pragma once
#include <memory>
#include <vector>


#include <Agent/TreeSearchAgents/ITreeNode.h>
#include <ForwardModel/TBSForwardModel.h>
#include <Representation/TBSGameState.h>


namespace SGA {

	class TreeNode : public ITreeNode<TreeNode> {
		
	private:
		TreeNode(TBSForwardModel& forwardModel, TBSGameState gameState, TreeNode* parent, int childIndex);

	public:
		// Root Node Constructor
		TreeNode(TBSForwardModel& forwardModel, TBSGameState gameState);
		TreeNode* expand(TBSForwardModel& forwardModel);
		
		//std::string toString() const override;
		void print() const override;
	};	

}
