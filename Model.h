//
// Created by Lauri Kortevaara(personal) on 08/11/15.
//

#ifndef OMEN_MODEL_H
#define OMEN_MODEL_H


#include "Mesh.h"

namespace omen {
    class Model {
    public:
        // ctor with pre-created mesh
        Model(std::unique_ptr<Mesh> mesh= nullptr);

        void render(const glm::mat4 &viewProjection, const glm::mat4 &view);
		std::unique_ptr<Mesh>& mesh() {
			return m_mesh;
		}
	private:
        std::unique_ptr<omen::Mesh> m_mesh;
    };
} // namespace omen

#endif //OMEN_MODEL_H
