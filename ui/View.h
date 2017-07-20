//
// Created by Lauri Kortevaara(Intopalo) on 06/02/16.
//

#ifndef OMEN_VIEW_H
#define OMEN_VIEW_H


#include <glm/glm.hpp>
#include "../Entity.h"

namespace omen {
	// Forward declarations
	class Color;

	namespace ecs {
		class Sprite;
	}

	// Enumerations and data classes
	namespace ui {
		

		enum ResizeMode{
			HORIZONTAL_RESIZE_ENABLED = 1,
			VERTICAL_RESIZE_ENABLED = 2,
			DIAGONAL_RESIZE_ENABLED = 4,
			NOT_RESIZABLE = 0,
			DEFAULT_RESIZE_MODE = NOT_RESIZABLE
		};

		enum MeasureSpec {
			AT_MOST = 1,
			EXACTLY = 2,
			UNSPECIFIED = 3
		};

		enum eViewState {
			ViewState_Normal, // Idle state, render normally
			ViewState_UpdatingLayout, // The layout is being updated. This prevents eternal loops when settings sizing
			ViewStatue_Dirty // Requires redaring and relayouting if layout
		};

		class LayoutParams {
		public:
			enum eSizing
			{
				Absolute,			 // Absolute pixel value
				DeviceIndependent,	 // Device independent measure value (dip or dp)
				WrapContent,		 // Takes as mucn space as content requires
				MatchParent,		 // Expands to parent size (width,height) 
				DefaultSizing = WrapContent
			};

			LayoutParams() : layoutSizingWidth(DefaultSizing), layoutSizingHeight(DefaultSizing) {}
			eSizing layoutSizingWidth;
			eSizing layoutSizingHeight;
			float	weight; // Affects linear layouts
		protected:
		private:

		};


		/*
		***
		* The View class
		**
		*/
        class View : public ecs::Entity {
        public:

        protected:

            View(View *parentView, const std::string& name, const glm::vec2& pos, const glm::vec2& size);

            virtual void onMeasure(MeasureSpec horintalMeas, MeasureSpec verticalMeas) = 0; // Called to determine the size requirements for this view and all of its children.
            virtual void onLayout(bool changed, float left, float top, float right, float bottom); // Called when this view should assign a size and position to all of its children. 
			virtual void onSizeChanged(glm::vec3 size, glm::vec3 oldSize); // Called when the size of this view has changed. 

        public:
            virtual ~View();

			virtual void updateLayout();

			virtual bool addChild(std::unique_ptr<omen::ecs::Entity> child);
			virtual bool removeChild(std::unique_ptr<omen::ecs::Entity> child);

            //void measureSize();

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
			glm::vec4 m_margins; // left, top, right, bottom; // How much margin this view has 

			glm::vec4 paddings() const { return m_paddings; }
			void setPaddings(const glm::vec4& paddings) { m_paddings = paddings; }
			glm::vec4 m_paddings; // left, top, right, bottom; // How much away from the edges the children are located
			
			virtual glm::vec3 pos() const { return Entity::pos() + glm::vec3(m_margins[left], m_margins[top], 0); }
			virtual glm::vec2 pos2D() const { return Entity::pos2D() + glm::vec2(m_margins[left], m_margins[top]); };

			virtual glm::vec3 localPos() const { return Entity::tr_const()->pos(); }
			virtual void setLocalPos(const glm::vec3& pos) { Entity::tr()->setPos(pos); }

			virtual glm::vec2 localPos2D() const { return glm::vec2(Entity::tr_const()->pos()); }
			virtual void setLocalPos2D(const glm::vec2& pos) { Entity::tr()->setPos(glm::vec3(pos, Entity::tr()->pos().z)); }

			virtual float width() const;
			virtual float height() const;

			virtual float minWidth() const { return m_minWidth >= 0 ? m_minWidth : 0.0f; }
			virtual float maxWidth() const { return m_maxWidth >= 0 ? m_maxWidth : width(); }
			virtual float minHeight() const { return m_minHeight >= 0 ? m_minHeight : 0.0f; }
			virtual float maxHeight() const { return m_maxHeight >= 0 ? m_maxHeight : height(); }

			virtual void setMinWidth(float minWidth) { m_minWidth = minWidth; }
			virtual void setMaxWidth(float maxWidth) { m_maxWidth = maxWidth; }
			virtual void setMinHeight(float minHeight) { m_minHeight = minHeight; }
			virtual void setMaxHeight(float maxHeight) { m_maxHeight = maxHeight; }

			glm::vec2 m_measuredSize;

			virtual float measuredWidth() const { return m_measuredSize.x; }
			virtual float measuredHeight() const { return m_measuredSize.y; }

			void setMeasuredSize(float width, float height) { m_measuredSize = glm::vec2(width, height); }
			virtual void measure(MeasureSpec horizontalMeas, MeasureSpec verticalMeas);


			bool resizable() const { return m_resizeMode != NOT_RESIZABLE; }
			bool hResizable() const { return m_resizeMode == HORIZONTAL_RESIZE_ENABLED; }
			bool vResizable() const { return m_resizeMode == VERTICAL_RESIZE_ENABLED; }
			bool dResizable() const { return m_resizeMode == DIAGONAL_RESIZE_ENABLED; }
			void setResizeMode(ResizeMode mode) { m_resizeMode = mode; }
			ResizeMode resizeMode() const { return m_resizeMode; }
			
			LayoutParams& layoutParams() { return m_layoutParams; }

			eViewState viewState() const { return m_viewState; }

			void setBackground(Color& color);
			void setBackground(std::unique_ptr<ecs::Sprite> sprite);

		private:
			float m_minWidth, m_maxWidth;   // Minimum and maximum width in absolute pixel value, if -1.0f then 0.0f is returned 
			float m_minHeight, m_maxHeight; // Minimum and maximum height in absolute pixel value, if -1.0f then 0.0f is returned
			ResizeMode m_resizeMode; // if true this view may be resized, otherwise not

			LayoutParams	m_layoutParams;
			eViewState		m_viewState;

		};
    } // namespace UI
} // namespace omen

#endif //OMEN_VIEW_H
