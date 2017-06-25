//
// Created by Lauri Kortevaara(Intopalo) on 06/02/16.
//

#ifndef OMEN_VIEW_H
#define OMEN_VIEW_H


#include <glm/glm.hpp>
#include <vector>
#include "../Signal.h"
#include "../Entity.h"

namespace omen {
    namespace ui {
		class LayoutParams {
		public:
			enum eLayoutSizing
			{
				Absolute,			 // Absolute pixel value
				DeviceIndependent,	 // Device independent measure value (dip or dp)
				WrapContent,		 // Takes as mucn space as content requires
				MatchParent,		 // Expands to parent size (width,height) 
				DefaultSizing = WrapContent
			};

			LayoutParams() : layoutSizingWidth(DefaultSizing), layoutSizingHeight(DefaultSizing) {}
			eLayoutSizing layoutSizingWidth;
			eLayoutSizing layoutSizingHeight;
		protected:
		private:

		};

        class View : public ecs::Entity {
        public:

        protected:

            View(View *parentView, const std::string& name, const glm::vec2& pos, const glm::vec2& size);

            virtual void updateLayout(); 
            virtual void onMeasure(float maxwidth, float maxheight) = 0; // Called to determine the size requirements for this view and all of its children.
            virtual void onLayout(bool changed, float left, float top, float right, float bottom); // Called when this view should assign a size and position to all of its children. 
			virtual void onSizeChanged(glm::vec3 size, glm::vec3 oldSize); // Called when the size of this view has changed. 
        public:
            virtual ~View();

			virtual bool addChild(std::unique_ptr<omen::ecs::Entity> child);
			virtual bool removeChild(std::unique_ptr<omen::ecs::Entity> child);

            void measureSize();

			View* parentView() const { return dynamic_cast<View*>(parent()); }

            // Layout
            void layout(float left, float top, float right, float bottom);

			glm::vec4 margins() const { return m_margins; }
			void setMargins(const glm::vec4& margins) { m_margins = margins; }
			
			enum MARGIN_POS {
				left = 0,
				top = 1,
				right = 2,
				bottom = 3
			};
			glm::vec4 m_margins; // left, top, right, bottom;
			
			virtual glm::vec3 pos() const { return Entity::pos() + glm::vec3(m_margins[left], m_margins[top], 0); }
			virtual glm::vec2 pos2D() const { return Entity::pos2D() + glm::vec2(m_margins[left], m_margins[top]); };

			virtual glm::vec3 localPos() const { return Entity::tr_const()->pos(); }
			virtual void setLocalPos(const glm::vec3& pos) { Entity::tr()->setPos(pos); }

			virtual glm::vec2 localPos2D() const { return glm::vec2(Entity::tr_const()->pos()); }
			virtual void setLocalPos2D(const glm::vec2& pos) { Entity::tr()->setPos(glm::vec3(pos, Entity::tr()->pos().z)); }

			virtual float width() const { return Entity::width();}
			virtual float height() const { return Entity::height(); }

			virtual float minWidth() const { return m_minWidth >= 0 ? m_minWidth : 0.0f; }
			virtual float maxWidth() const { return m_maxWidth >= 0 ? m_maxWidth : width(); }
			virtual float minHeight() const { return m_minHeight >= 0 ? m_minHeight : 0.0f; }
			virtual float maxHeight() const { return m_maxHeight >= 0 ? m_maxHeight : height(); }

			virtual void setMinWidth(float minWidth) { m_minWidth = minWidth; }
			virtual void setMaxWidth(float maxWidth) { m_maxWidth = maxWidth; }
			virtual void setMinHeight(float minHeight) { m_minHeight = minHeight; }
			virtual void setMaxHeight(float maxHeight) { m_maxHeight = maxHeight; }
			
			LayoutParams& layoutParams() { return m_layoutParams; }
		private:
			float m_minWidth, m_maxWidth;   // Minimum and maximum width in absolute pixel value, if -1.0f then 0.0f is returned 
			float m_minHeight, m_maxHeight; // Minimum and maximum height in absolute pixel value, if -1.0f then 0.0f is returned
			

			LayoutParams m_layoutParams;

		};
    } // namespace UI
} // namespace omen

#endif //OMEN_VIEW_H
