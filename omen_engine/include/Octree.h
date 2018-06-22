#pragma once
#include <glm/glm.hpp>
namespace omen {
	class Mesh;
	class Octree {
	public:

		Octree(Mesh* mesh);

		const float minSize = 0.2f;

		class Node {
		public:
			Node();
			Node* parent;
			Node* children[8];
			float size;
			glm::vec3 pos;

			/* Is this node inside of given node*/
			bool isInside(Node* n);

			bool addChild(Node* node);

		private:
		protected:
		};

		Node* rootNode;

		Node* findNode(const float& x, const float& y, const float& z, Node* node = nullptr);
		Node* getParentNode(Node* n, Node* parentNodeCandidate = nullptr);
		Node* addNodeTo(const float& x, const float& y, const float& z);
		Node* addNodeTo(const glm::vec3& pos);

		void buildTree(Mesh* mesh);
	protected:
	private:
	};
}


