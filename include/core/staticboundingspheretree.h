#pragma once

#include <glm/glm.hpp>
#include <model/geo/sphere.h>
#include <model/geo/plane.h>
#include <model/withboundingsphere.h>
#include <model/geo/interactions.h>

#include <vector>
#include <memory>
#include <algorithm>

/**
 * Static bounding sphere tree
 *
 * Heirarchial representation of a STATIC group of input bounding spheres
 */

namespace core
{
	template <class WithBoundingSphereType>
	class StaticBoundingSphereTree
	{
	protected:
		struct TreeNode
		{
			enum NODE_TYPE
			{
				LEAF, BRANCH
			};

			NODE_TYPE Type;
			union
			{
				std::shared_ptr<WithBoundingSphereType> Element;
				std::shared_ptr<TreeNode> Left;
			};
			std::shared_ptr<TreeNode> Right;
			model::geo::Sphere ContainingSphere;
		};

	public:
		class InvalidInputException {};
		class EmptyElementListException : public InvalidInputException {};

		static_assert(std::is_base_of<WithBoundingSphereType, WithBoundingSphereType>::value, "ElementType must be a base class of WithBoundingSphereType");
	public:
		StaticBoundingSphereTree(const std::vector<std::shared_ptr<WithBoundingSphereType>>& elements)
			: root_(nullptr)
		{
			build(elements);
		}

		StaticBoundingSphereTree(const StaticBoundingSphereTree&) = delete;
		~StaticBoundingSphereTree() = default;

		std::vector<std::shared_ptr<WithBoundingSphereType>> queryWithinPlanes(const std::vector<model::geo::Plane>& planes)
		{
			std::vector<std::shared_ptr<StaticBoundingSphereTree::TreeNode>> nodesToTry;
			std::vector<std::shared_ptr<WithBoundingSphereType>> toReturn;
			nodesToTry.reserve(32u);
			nodesToTry.push_back(root_);

			while (nodesToTry.size() > 0u)
			{
				auto nextNode = nodesToTry[0];
				nodesToTry.erase(nodesToTry.begin());

				bool isWithinPlanes = true;
				for (auto&& p : planes)
				{
					isWithinPlanes &= model::geo::isSphereAtAllInPlane(p, nextNode->ContainingSphere);
				}

				if (isWithinPlanes)
				{
					if (nextNode->Type == StaticBoundingSphereTree::TreeNode::LEAF)
					{
						toReturn.push_back(nextNode->Element);
					}
					else
					{
						nodesToTry.push_back(nextNode->Left);
						nodesToTry.push_back(nextNode->Right);
					}
				}
			}

			return toReturn;
		}

	protected:
		void sortAlongX(std::vector<std::shared_ptr<WithBoundingSphereType>>& elements)
		{
			std::sort(elements.begin(), elements.end(), [](std::shared_ptr<WithBoundingSphereType> t1, std::shared_ptr<WithBoundingSphereType> t2) {
				auto a = t1->getBoundingSphere();
				auto b = t2->getBoundingSphere();

				auto ax = a.origin().x;
				auto bx = b.origin().x;

				return ax > bx;
			});
		}

		void build(const std::vector<std::shared_ptr<WithBoundingSphereType>>& elements)
		{
			if (elements.size() == 0u)
			{
				throw EmptyElementListException();
			}

			std::vector<std::shared_ptr<TreeNode>> processingLevel;
			auto toProcess = elements;
			sortAlongX(toProcess);

			// First, make a TREE_NODE of all current elements
			for (auto&& element : toProcess)
			{
				auto tn = std::make_shared<TreeNode>();
				tn->Type = TreeNode::LEAF;
				tn->Element = element;
				tn->Right = nullptr;
				tn->ContainingSphere = element->getBoundingSphere();
				processingLevel.push_back(tn);
			}

			// Next, while we have any in the processing level, put them all into the next level
			while (processingLevel.size() > 1u)
			{
				// Combine every two in the processing level into one in the next level
				std::vector<std::shared_ptr<StaticBoundingSphereTree::TreeNode>> nextLevel;
				while (processingLevel.size() >= 2u)
				{
					auto first = processingLevel[0];
					auto second = processingLevel[1];

					auto combined = std::make_shared<TreeNode>();
					combined->Type = TreeNode::BRANCH;
					combined->Left = first;
					combined->Right = second;
					combined->ContainingSphere = model::geo::Sphere::getCombinedSphere(first->ContainingSphere, second->ContainingSphere);

					nextLevel.push_back(combined);

					processingLevel.erase(processingLevel.begin());
					processingLevel.erase(processingLevel.begin() + 1u);
				}

				// If there are any left, just add them to the next level...
				for (auto&& p : processingLevel)
				{
					nextLevel.push_back(p);
				}

				processingLevel = nextLevel;
			}

			root_ = processingLevel[0];
		}

	private:
		std::shared_ptr<TreeNode> root_;
	};
}
