#pragma once

#include "components/simple_scene.h"
#include "lab_m1/Train/camera.h"
#include "lab_m1/Train/train.h"
#include "lab_m1/Train/renderable.h"
#include "components/text_renderer.h"
struct InteractiveStation {
    glm::vec3 position;
    ResourceType resource;
    float cooldownTimer;   // if > 0, empty for now
    float radius;
};

    namespace m1
{
    class TrainGame : public gfxc::SimpleScene
    {
    public:
        TrainGame();
        ~TrainGame();

        void GenerateNewOrder();
        void CheckStationInteractions(float deltaTime);
        void RenderCommandQueue(const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix);
        void DrawScene(const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix);
        // MESH INITIALIZATION PARAMETERS
        const float UNIT_RADIUS = 1.0f;
        const float UNIT_LENGTH = 1.0f;
        const float ENGINE_MESH_RADIUS = 0.5f;
        const float ENGINE_MESH_LENGTH = 1.0f;

        // COLORS
        const glm::vec3 LOCO_WHEEL_COLOR = glm::vec3(1, 0, 0);       // Red
        const glm::vec3 LOCO_BASE_COLOR = glm::vec3(1, 1, 0);        // Yellow
        const glm::vec3 LOCO_CABIN_COLOR = glm::vec3(0, 1, 0);       // Green
        const glm::vec3 LOCO_ENGINE_COLOR = glm::vec3(0, 0, 0.6f);   // Dark Blue
        const glm::vec3 LOCO_INNER_ENGINE_COLOR = glm::vec3(0.5f, 0, 0.5f); // Purple
        void Init() override;

        void InitMapMeshes();

        void InitTrainMeshes();
        Rail* AddTrackSegment(glm::vec3 start, glm::vec3 end, float rotation, RailType type = RAIL_NORMAL);
    private:
        float cooldownAmount = 5.0f;
        Camera* minimapCamera;
        glm::mat4 minimapProjection;
        gfxc::TextRenderer* textRenderer;
        std::vector<ResourceType> currentOrder;
        std::vector<InteractiveStation> stations;
        float gameTimeRemaining;
        const float TIME_LIMIT = 60.0f;
        bool isGameOver;
        int score;
        glm::vec3 colorWood = glm::vec3(0.9f, 0.6f, 0.1f);
        glm::vec3 colorOil = glm::vec3(0.1f, 0.1f, 0.1f);
        glm::vec3 colorStone = glm::vec3(0.5f, 0.5f, 0.5f);
        glm::vec3 colorCentralBase = glm::vec3(0.8f, 0.2f, 0.2f);
        void CreateTrack();
        void ConnectIntersections(Intersection* A, Intersection* B, RailType type);
        void RenderMeshCustom(
            Mesh* mesh,
            Shader* shader,
            const glm::mat4& modelMatrix,
            const glm::mat4& viewMatrix,
            const glm::mat4& projectionMatrix);
        void RenderScene(float deltaTimeSeconds);
        void FrameStart() override;
        void Update(float deltaTimeSeconds) override;
        void ChooseDifficulty();
        void UpdateLogic(float deltaTimeSeconds);
        void FrameEnd() override;

        void OnInputUpdate(float deltaTime, int mods) override;
        void OnKeyPress(int key, int mods) override;
        void OnKeyRelease(int key, int mods) override;
        void OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY) override;
        void OnMouseBtnPress(int mouseX, int mouseY, int button, int mods) override;
        void OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods) override;
        void OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY) override;
        void OnWindowResize(int width, int height) override;

    protected:
        int gameStarted = 0;
        bool isFreeCamera;
        std::vector<Intersection*> allIntersections;
        Train* train;
        Camera* camera;
        bool renderCameraTarget;
        float left = -10.0f, right = 10.0f, bottom = -10.0f, top = 10.f, zNear = 0.1f, zFar = 100.0f;
        float fov = 80.0f;
        std::vector<Rail*> allRails;
    };
}   // namespace m1
