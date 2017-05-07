//
// Created by Lauri Kortevaara on 26/12/15.
//

#ifndef OMEN_ENTITY_H
#define OMEN_ENTITY_H

#include <string>
#include <stdint.h>
#include <vector>
#include <memory>

#include "Object.h"
#include "component/Transform.h"

namespace omen {
    namespace ecs {
        class Component;
		class Entity : public omen::Object {

			Entity* m_parent;
			std::vector<std::unique_ptr<Entity>> m_children;
			std::vector<std::unique_ptr<Component>> m_components;
			int m_layer;

			glm::vec2 m_deltaPos;
			glm::vec2 m_cursorPos;
			bool m_is_hovered;
			bool m_is_pressed;
			bool m_is_selected;

		public:
			typedef omen::Signal<std::function<void(Entity *, glm::vec2)> > SignalHovered_t;
			typedef omen::Signal<std::function<void(Entity *, glm::vec2)> > SignalEntered_t;
			typedef omen::Signal<std::function<void(Entity *, glm::vec2)> > SignalExited_t;
			typedef omen::Signal<std::function<void(Entity*) > > Entity_Destructed_t;

			SignalHovered_t signal_hovered;
			SignalEntered_t signal_entered;
			SignalExited_t  signal_exited;
			Entity_Destructed_t signal_entity_destructed;

		public:
			Entity(const std::string &name);
			virtual ~Entity();

			Entity* parent() const { return m_parent; }
			void setParent(Entity* parent) { m_parent = parent; }

			void setSelected(bool selected) { m_is_selected = selected;  }

            std::vector<std::unique_ptr<Entity>>& children() {return m_children;}
			const std::vector<std::unique_ptr<Entity>>& children_const() const { return m_children; }
            virtual bool addChild(std::unique_ptr<Entity> e);
            bool removeChild(std::unique_ptr<Entity> e);

            bool removeComponent(std::unique_ptr<Component> c);

			Entity* findChild(const std::string& name);
			Entity const* findChild_const(const std::string& name) const;

			void setLayer(int layer) { m_layer = layer; }
			int layer() const { return m_layer; }

			Transform* tr() { return getComponent<Transform>(); }
			const Transform* tr_const() const { return getComponent_const<Transform>(); }

			enum Gravity : int {
				ALIGN_LEFT = (1u << 0),
				ALIGN_RIGHT = (1u << 1),
				ALIGN_TOP = (1u << 2),
				ALIGN_BOTTOM = (1u << 3),
				VERTICAL_CENTER = (1u << 4),
				HORIZONTAL_CENTER = (1u << 5)
			};

			int m_gravity;
			int gravity() const { return m_gravity; }
			void setGravity(int g) { m_gravity = g; }

			virtual glm::vec3 pos() const;
			virtual glm::vec2 pos2D() const;

			virtual glm::vec3 localPos() const { return Entity::tr_const()->pos(); }
			virtual void setLocalPos(const glm::vec3& pos) { Entity::tr()->setPos(pos); }

			virtual glm::vec2 localPos2D() const { return glm::vec2(Entity::tr_const()->pos()); }
			virtual void setLocalPos2D(const glm::vec2& pos) { Entity::tr()->setPos(glm::vec3(pos,Entity::tr()->pos().z)); }

			virtual float width() const { return tr_const()->boundsMax().x - tr_const()->boundsMin().x; }
			virtual float height() const { return tr_const()->boundsMax().y - tr_const()->boundsMin().y; }
			virtual float depth() const { return tr_const()->boundsMax().z - tr_const()->boundsMin().z; }

			virtual glm::vec3 size() const { return glm::vec3(width(), height(), depth()); }
			virtual glm::vec2 size2D() const { return glm::vec2(width(), height()); }

			virtual void setSize(const glm::vec3& size) { tr()->setBounds(tr()->boundsMin(), tr()->boundsMin() + size); }
			virtual void setSize2D(const glm::vec2& size) { tr()->setBounds(tr()->boundsMin(), tr()->boundsMin() + glm::vec3(size.x,size.y,0)); }

			bool hovered() const { return m_is_hovered; }
			bool pressed() const { return m_is_pressed; }
			void setPressed(bool pressed) { m_is_pressed = pressed;}
			bool selected() const { return m_is_selected; }

        public:
            template<class type>
			type* getComponent(const std::string &component_name = "") {
				for (auto& c : m_components)
					if (c.get() != nullptr) {
						type* ptr = dynamic_cast<type*>(c.get());
						if(ptr != nullptr)
							return ptr;
					}
				return nullptr;
			}

			template<class type>
			type const* getComponent_const(const std::string &component_name = "") const {
				for (auto const& c : m_components)
					if (c.get() != nullptr) {
						type const* ptr = dynamic_cast<type const*>(c.get());
						if (ptr != nullptr)
							return ptr;
					}
				return nullptr;
			}

            bool addComponent(std::unique_ptr<Component> c);
        };
    } // namespace ecs
} // namespace omen

#endif //OMEN_ENTITY_H
