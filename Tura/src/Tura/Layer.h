#pragma once

namespace Tura {

	class Layer
	{
		bool m_Inited = false;

	public:
		virtual ~Layer() = default;

		virtual void OnUpdate(float aDeltaTIme) {}
		virtual void OnAttach() { m_Inited = true; }
		virtual void OnDetach() {}

		virtual bool OnUIRender() { return true; }


		bool HasInited()
		{
			if (!m_Inited)
			{
				m_Inited = true;;
				return false;
			}

			return true;
		}
	};

}