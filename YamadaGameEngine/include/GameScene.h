#pragma once
#include "include/IScene.h"
#include "include/exampleEntity.h"

// GameScene��IScene���������A�G���e�B�e�B�ƃV�X�e���̊Ǘ����s���Q�[���̎�v�V�[���N���X�ł��B
// �G���e�B�e�B�̐����A�폜�A�X�V�A����уJ�����Ǘ��@�\��񋟂��܂��B
// �����̃V�X�e����o�^���ăV�[���S�̂̏����𐧌䂵�܂��B


class GameScene : public IScene
{
public:
    GameScene() {};
    ~GameScene() = default;;

    void Initialize() override;
    void Update(float deltaTime) override;
    void Cleanup() override;

    template<typename T, typename... Args, typename = std::enable_if_t<std::is_base_of_v<IEntity, T>>>
    IEntity* CreateEntity(Args&&... args) {
        auto entity = std::make_unique<T>(std::forward<Args>(args)...);  // ������n���Đ���
        IEntity* rawPtr = entity.get();
        m_entities.push_back(std::move(entity));
        return rawPtr;
    }

    void RemoveEntity(IEntity* entity) override;
    const std::vector<std::unique_ptr<IEntity>>& GetEntities() const override { return m_entities; }

    void RemoveDestroyedEntities();

    // System �Ǘ�
    void AddSystem(ISystem* system) override 
    {
        m_systems.push_back(system);
    }
    void RemoveSystem(ISystem* system) override;
    const std::vector<ISystem*>& GetSystems() const override { return m_systems; }

	// �J�����̎擾
	IEntity* GetMainCameraEntity() const {// �J�����������Ă���Entity��Ԃ��B
        return m_mainCamera;
    }
    void SetMainCameraEntity(IEntity* camera) override;

    AssetManager& GetAssetManager() override { return assetManager; }

private:
    std::vector<std::unique_ptr<IEntity>> m_entities;
    std::vector<ISystem*> m_systems;
    IEntity* m_mainCamera = nullptr;
    AssetManager assetManager;
};
