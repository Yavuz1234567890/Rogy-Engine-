#pragma once

#ifndef R_OBJ_H
#define R_OBJ_H

#include "transform.h"
#include "RTransform.h"
#include "RComponent.h"

#include "../physics/RigidBody.h"

#include "../renderer/rendererComponent.h"
#include "../renderer/lighting.h"
#include "../renderer/reflection_probe.h"
#include "../renderer/billboardComponent.h"
#include "../renderer/cameraComponent.h"
#include "../renderer/ParticleSystem.h"
#include "../audio/AudioSource.h"
#include "../renderer/GrassComponent.h"

#include <iostream>
#include <vector>
#include <string>

#include "../scripting/ScriptManager.h"

#include "../io/FSerializer.h"
#include "../scripting/LuaScriptSerializer.h"

typedef unsigned int EnttID;                                                       

enum EntityEvent
{
	EE_ON_COLLISION_ENTER,
	EE_ON_COLLISION_EXIT,
};

class Entity
{
public:
	// Entity Properties
	//-----------------------------------------------
	
	EnttID ID = -1;
	std::string name;
	std::string path;

	Entity* parent = nullptr;
	std::string tag;

	bool Static      = false;
	bool Active      = true;
	bool is_Selected = false;
	bool is_prefab   = false;

	bool root = false; // Only true in scene root entity

public:
	// each entity contain childern entities and components
	//-----------------------------------------------
	Transform transform;
	
	std::vector<Component*> m_components;
	std::vector<ScriptInstance*> m_scripts;

	std::vector<Entity*> Children;

	std::vector<Entity*> all_children;

	// Functions
	//-----------------------------------------------

	Entity();
	~Entity();

	bool IsColliding = false;
	void OnCollision(bool enter, Entity* trgt);

	bool AddScript(ScriptInstance* scr);
	bool RemoveScript(std::string& scrName);
	ScriptInstance* GetScript(std::string& scrName);
	int GetScriptInstance(std::string& scrName);
	void InvokeScriptFunc(const char* funcName);
	void StartScripts();

	unsigned int ChildCount()
	{
		return Children.size();
	}

	std::vector<Entity*> GetChildren()
	{
		return Children;
	};

	template<typename T>
	bool AddComponent(Component* comp)
	{
		if (!comp) return false;

		comp->type_id = T::TYPE_ID;
		m_components.push_back(comp);
		return true;
	}

	template<typename T>
	T* GetComponent()
	{
		for (size_t i = 0; i < m_components.size(); i++)
		{
			if (m_components[i]->type_id == T::TYPE_ID)
			{
				return static_cast<T*>(m_components[i]);
			}
		}
		return nullptr;
	}

	template<typename T>
	bool HasComponent()
	{
		for (size_t i = 0; i < m_components.size(); i++)
		{
			if (m_components[i]->type_id == T::TYPE_ID)
			{
				return true;
			}
		}
		return false;
	}

	template<typename T>
	bool RemoveComponent()
	{
		for (size_t i = 0; i < m_components.size(); i++)
		{
			if (m_components[i]->type_id == T::TYPE_ID)
			{
				m_components[i]->removed = true;
				m_components.erase(m_components.begin() + i);
				return true;
			}
		}
		return false;
	}

	int GetObjIndex(EnttID obj_id);

	void MovePlace(bool up = true);

	void MoveChildPlace(EnttID entId, bool up = true);

	void SetTranslation(glm::vec3 pos, bool local = false);
	void SetRotation(glm::vec3 angels);
	void SetScale(glm::vec3 newScale);
	void RotateY(float angle);

	/* Set entity parent. */
	bool SetParent(Entity* target);

	/* Add an existing child to this -change parent of the target entity to this- */
	Entity* AddChild(Entity* mObj);

	/* Create a new child on this entity */
	Entity* AddChild(std::string ent_name, EnttID ent_id);

	void DestroySelfIn(float tm);
	void DestroySelf();

	/* Removes an existing child from this entity and delete its memory. */
	void RemoveChild(EnttID o_id);

	/* Removes an existing child from this entity without releasing its memory
	   usually for parent changing, so always use RemoveChild instead. */
	void RemoveChild_F(EnttID o_id);

	/* Removes all children and deletes used memory */
	void RemoveAllChilds();

	/* Check if this entity contain a child */
	bool HasChild(EnttID child_id);

	void Start();
	void Update(float dt);
	void OnDestory();

	bool IsRoot();

	/* Find an entity in this entity or its children. */
	Entity* Find_entity(EnttID obj_id, Entity* in_obj);
	Entity* Find_entityN(std::string obj_name, Entity* in_obj);
	Entity* GetChild(std::string obj_name);

	bool SaveEntityFile(const char* pth);

	bool IsPrefab();

	void SaveBP(cereal::BinaryOutputArchive& ar, bool is_root);

	bool started = false;
	bool bboxSeted = false;
	bool doRBActive = false;
private:
	bool willDestroy = false;
	float destroyIn = 0.0f;
	
};
#endif