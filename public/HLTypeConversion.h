//
// AMX Mod X, based on AMX Mod by Aleksander Naszko ("OLO").
// Copyright (C) The AMX Mod X Development Team.
//
// This software is licensed under the GNU General Public License, version 3 or higher.
// Additional exceptions apply. For full license details, see LICENSE.txt or visit:
//     https://alliedmods.net/amxmodx-license

#ifndef _HL_CONVERSION_TYPE_H_
#define _HL_CONVERSION_TYPE_H_

#include "amxxmodule.h"

template <typename T> static inline T& ref_pdata(void *pPrivateData, int offset, int element = 0)
{
	return *reinterpret_cast<T*>((reinterpret_cast<int8*>(pPrivateData) + offset + (element * sizeof(T))));
}


template <typename T> inline T get_pdata_direct(edict_t *pEntity, int offset, int element = 0, int size = 0)
{
	return reinterpret_cast<T>((reinterpret_cast<int8*>(pEntity->pvPrivateData) + offset + (element * size)));
}

template <typename T> inline T& get_pdata(void *pPrivateData, int offset, int element = 0)
{
	return ref_pdata<T>(pPrivateData, offset, element);
}

template <typename T> inline T& get_pdata(edict_t *pEntity, int offset, int element = 0)
{
	return get_pdata<T>(pEntity->pvPrivateData, offset, element);
}


template <typename T> inline void set_pdata(void *pPrivateData, int offset, T value, int element = 0)
{
	ref_pdata<T>(pPrivateData, offset, element) = value;
}

template <typename T>inline void set_pdata(edict_t *pEntity, int offset, T value, int element = 0)
{
	set_pdata<T>(pEntity->pvPrivateData, offset, value, element);
}


class HLTypeConversion
{
	public:

		HLTypeConversion() : m_FirstEdict(nullptr), m_PevOffset(0)
		{}

	public:

		void init()
		{
			if (!m_FirstEdict)
			{
				m_FirstEdict = INDEXENT(0);

				search_pev();
			}
		}

	public: // Edict -> Index

		int edict_to_id(edict_t *pEdict)
		{
			if (FNullEnt(pEdict))
			{
				return -1;
			}

			return static_cast<int>(pEdict - m_FirstEdict);
		}

	public: // Entvars -> Edict/Index

		edict_t* entvar_to_edict(entvars_t *pev)
		{
			if (!pev || FNullEnt(pev->pContainingEntity))
			{
				return nullptr;
			}

			return pev->pContainingEntity;
		}

		int entvars_to_id(entvars_t *pev)
		{
			return edict_to_id(entvar_to_edict(pev));
		}

	public: // Index -> CBase*/Edict/Entvars

		void* id_to_cbase(int index)
		{
			edict_t *pEdict = id_to_edict(index);
			return pEdict ? pEdict->pvPrivateData : nullptr;
		}

		edict_t* id_to_edict(int index)
		{
			return static_cast<edict_t*>(m_FirstEdict + index);
		}

		entvars_t* id_to_entvars(int index)
		{
			edict_t *pEdict = id_to_edict(index);
			return pEdict ? VARS(pEdict) : nullptr;
		}

	public: // CBase* -> Entvars/Index

		entvars_t* cbase_to_entvar(void *cbase)
		{
			if (!cbase)
			{
				return nullptr;
			}

			return get_pdata<entvars_t*>(cbase, m_PevOffset);
		}

		int cbase_to_id(void *cbase)
		{
			return entvars_to_id(cbase_to_entvar(cbase));
		}

	private:

		void search_pev()
		{
			entvars_t *pev = VARS(m_FirstEdict);

			byte *privateData = reinterpret_cast<byte*>(m_FirstEdict->pvPrivateData);

			for (size_t i = 0; i < 0xFFF; ++i)
			{
				entvars_t *val = *(reinterpret_cast<entvars_t**>(privateData + i));

				if (val == pev)
				{
					m_PevOffset = i;
					return;
				}
			}

			m_PevOffset = 0; // This should not happen.
		}

	private:

		edict_t* m_FirstEdict;
		size_t   m_PevOffset;
};

class EHANDLE
{
	private:

		edict_t* m_pent;
		int		 m_serialnumber;

	public:

		edict_t* Get(void)
		{
			if (!FNullEnt(m_pent))
			{
				if (m_pent->serialnumber == m_serialnumber)
				{
					return m_pent;
				}

				return nullptr;
			}

			return nullptr;
		};

		edict_t* Set(edict_t *pent)
		{
			if (!FNullEnt(pent))
			{
				m_pent = pent;
				m_serialnumber = m_pent->serialnumber;
			}
			else
			{
				m_pent = nullptr;
				m_serialnumber = 0;
			}

			return pent;
		};
};

#endif // _HL_CONVERSION_TYPE_H_