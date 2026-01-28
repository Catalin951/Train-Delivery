#include "lab_m1/Train/trainGame.h"
#include "lab_m1/Train/camera.h"
#include "lab_m1/Train/transform3D.h"
#include "lab_m1/Train/object3D.h"
#include "components/text_renderer.h"

#include <vector>
#include <string>
#include <iostream>

using namespace std;
using namespace m1;

TrainGame::TrainGame()
{
}


TrainGame::~TrainGame()
{
}


void TrainGame::Init()
{
    isFreeCamera = false;
    renderCameraTarget = false;
    camera = new Camera();
    camera->Set(glm::vec3(50, 20, 0), glm::vec3(0, 1, 0), glm::vec3(0, 1, 0));
    train = new Train(glm::vec3(0, 0, 30), -90);
    InitTrainMeshes();
    InitMapMeshes();
    camera->SetProjectionMatrix(glm::perspective(RADIANS(60), window->props.aspectRatio, 0.01f, 200.0f));
    CreateTrack();
    for (auto rail : allRails) {
        if (glm::distance(rail->startPosition, glm::vec3(0, 0, 30)) < 0.1f &&
            glm::distance(rail->endPosition, glm::vec3(0, 0, 60)) < 0.1f) {
            train->SetRail(rail);
            break;
        }
    }
    gameTimeRemaining = TIME_LIMIT;
    isGameOver = false;
    score = 0;
    stations.push_back({ glm::vec3(0, 0, 0), ResourceType::NONE, 0.0f, 3.0f });
    stations.push_back({ glm::vec3(-70, 0, -60), ResourceType::WOOD, 0.0f, 3.0f });
    stations.push_back({ glm::vec3(70, 0, -60), ResourceType::OIL, 0.0f, 3.0f });
    stations.push_back({ glm::vec3(0, 0, 80), ResourceType::STONE, 0.0f, 3.0f });
    GenerateNewOrder();

    glm::ivec2 resolution = window->GetResolution();
    textRenderer = new gfxc::TextRenderer(window->props.selfDir, resolution.x, resolution.y);
    textRenderer->Load(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::FONTS, "Hack-Bold.ttf"), 48);
    minimapCamera = new Camera();
    minimapCamera->Set(glm::vec3(0, 70, 0), glm::vec3(0, 0, 0), glm::vec3(0, 0, -1));
    float orthoSize = 100.0f; // adjust for zoom
    minimapProjection = glm::ortho(-orthoSize, orthoSize, -orthoSize, orthoSize, 0.01f, 200.0f);
}

void TrainGame::InitMapMeshes() {
    Mesh* ground = object3D::CreateParallelepiped("ground", glm::vec3(0, 0, 0), 1.0f, 1.0f, 1.0f, glm::vec3(0.2f, 0.6f, 0.2f));
    AddMeshToList(ground);

    Mesh* water = object3D::CreateParallelepiped("ground_water", glm::vec3(0, 0, 0), 1.0f, 1.0f, 1.0f, glm::vec3(0.0f, 0.2f, 0.8f));
    AddMeshToList(water);

    Mesh* mountain = object3D::CreateParallelepiped("ground_mountain", glm::vec3(0, 0, 0), 1.0f, 1.0f, 1.0f, glm::vec3(0.4f, 0.2f, 0.1f));
    AddMeshToList(mountain);

    Mesh* sleeper = object3D::CreateParallelepiped("sleeper", glm::vec3(0, 0, 0), 1.0f, 1.0f, 1.0f, glm::vec3(0.2f, 0.1f, 0.0f));
    AddMeshToList(sleeper);

    Mesh* tunnel = object3D::CreateParallelepiped("tunnel_block", glm::vec3(0, 0, 0), 1.0f, 1.0f, 1.0f, glm::vec3(0.3f, 0.2f, 0.1f));
    AddMeshToList(tunnel);

    Mesh* bridge = object3D::CreateParallelepiped("bridge_beam", glm::vec3(0, 0, 0), 1.0f, 1.0f, 1.0f, glm::vec3(0.7f, 0.7f, 0.7f));
    AddMeshToList(bridge);

    Mesh* metal = object3D::CreateParallelepiped("metal_rail", glm::vec3(0, 0, 0), 1.0f, 1.0f, 1.0f, glm::vec3(0.55f, 0.60f, 0.65f));
    AddMeshToList(metal);

    // Central station meshes
    for (int i = 0; i < 10; i++) {
        float colorVal = (float)i / 9.0f;
        glm::vec3 color = glm::vec3(1.0f, colorVal, colorVal);
        // unique name: "central_0", "central_1", ... "central_9"
        std::string name = "central_" + std::to_string(i);
        Mesh* m = object3D::CreateParallelepiped(name, glm::vec3(0, 0, 0), 1.0f, 1.0f, 1.0f, color);
        AddMeshToList(m);
    }

    Mesh* resourceSt1 = object3D::CreateParallelepiped("station_wood", glm::vec3(0, 0, 0), 1.0f, 1.0f, 1.0f, glm::vec3(0.9f, 0.6f, 0.1f));
    AddMeshToList(resourceSt1);

    Mesh* resourceSt2 = object3D::CreateCylinder("station_oil", glm::vec3(0, 0, 0), 0.5f, 1.0f, glm::vec3(0.1f, 0.4f, 0.8f));
    AddMeshToList(resourceSt2);

    Mesh* resourceSt3 = object3D::CreateSphere("station_stone", glm::vec3(0, 0, 0), 1.0f, glm::vec3(0.047f, 0.761f, 0.667f));
    AddMeshToList(resourceSt3);
}

void TrainGame::InitTrainMeshes() {
    // Train Wheel
    Mesh* trainWheelMesh = object3D::CreateCylinder("trainWheel", glm::vec3(0, 0, 0), UNIT_RADIUS, UNIT_LENGTH, LOCO_WHEEL_COLOR);
    AddMeshToList(trainWheelMesh);

    // Train Base
    Mesh* trainBaseMesh = object3D::CreateParallelepiped("trainBase", glm::vec3(0, 0, 0), UNIT_LENGTH, UNIT_LENGTH, UNIT_LENGTH, LOCO_BASE_COLOR);
    AddMeshToList(trainBaseMesh);

    // Train Cabin
    Mesh* trainCabinMesh = object3D::CreateParallelepiped("trainCabin", glm::vec3(0, 0, 0), UNIT_LENGTH, UNIT_LENGTH, UNIT_LENGTH, LOCO_CABIN_COLOR);
    AddMeshToList(trainCabinMesh);

    // Train Engine
    Mesh* trainEngineMesh = object3D::CreateCylinder("trainEngine", glm::vec3(0, 0, 0), ENGINE_MESH_RADIUS, ENGINE_MESH_LENGTH, LOCO_ENGINE_COLOR);
    AddMeshToList(trainEngineMesh);

    // Train Inner Engine
    Mesh* trainInnerMesh = object3D::CreateCylinder("trainInnerEngine", glm::vec3(0, 0, 0), ENGINE_MESH_RADIUS, ENGINE_MESH_LENGTH, LOCO_INNER_ENGINE_COLOR);
    AddMeshToList(trainInnerMesh);

}
void TrainGame::GenerateNewOrder() {
    currentOrder.clear();
    for (int i = 0; i < 5; i++) {
        int random = rand() % 3 + 1; // (Wood, Oil, Stone)
        currentOrder.push_back((ResourceType)random);
    }
    // RESET TIME
    gameTimeRemaining = TIME_LIMIT;
    printf("Comanda noua generata!\n");
}

void TrainGame::CheckStationInteractions(float deltaTime) {
    if (isGameOver)
        return;
    gameTimeRemaining -= deltaTime;
    if (gameTimeRemaining <= 0) {
        isGameOver = true;
        printf("GAME OVER! Score: %d\n", score);
        return;
    }
    for (auto& s : stations)
        if (s.cooldownTimer > 0)
            s.cooldownTimer -= deltaTime;

    // Verify Train - station
    for (auto& s : stations) {
        float dist = glm::distance(train->position, s.position);
        if (dist > s.radius)
            continue;
        // Resource station and has no cooldown
        if (s.resource != ResourceType::NONE && s.cooldownTimer <= 0) {
            // No cooldown
            for (int i = 0; i < currentOrder.size(); i++)
                if (currentOrder[i] == s.resource) {
                    currentOrder.erase(currentOrder.begin() + i);
                    s.cooldownTimer = cooldownAmount;
                    printf("Colectat resursa! Mai ai: %d\n", currentOrder.size());
                    break;
                }
        }
        // Central station
        else if (s.resource == ResourceType::NONE && currentOrder.empty()) {
            score++;
            printf("Comanda Livrata! Scor: %d\n", score);
            GenerateNewOrder();
        }
    }
}

void TrainGame::FrameStart()
{
    // Clears the color buffer (using the previously set color) and depth buffer
    glClearColor(0.2235f, 0.6745f, 0.9059f, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::ivec2 resolution = window->GetResolution();
    // Sets the screen area where to draw
    glViewport(0, 0, resolution.x, resolution.y);
}

void TrainGame::Update(float deltaTimeSeconds)
{
    if (!gameStarted) {
        ChooseDifficulty();
    } else {
        UpdateLogic(deltaTimeSeconds);   
        RenderScene(deltaTimeSeconds);
    }
}
void TrainGame::ChooseDifficulty() {
    textRenderer->RenderText("Pick a difficulty 1 - 5 to start the game!", 20, 30, 1.0f, glm::vec3(1));
}

void TrainGame::RenderScene(float deltaTimeSeconds) {
    glm::ivec2 resolution = window->GetResolution();
    glViewport(0, 0, resolution.x, resolution.y);
    DrawScene(camera->GetViewMatrix(), camera->GetProjectionMatrix());
    int miniSize = resolution.y / 3;
    int miniX = resolution.x - miniSize - 20;
    int miniY = 20;
    glClear(GL_DEPTH_BUFFER_BIT);
    glViewport(miniX, miniY, miniSize, miniSize);
    if (train) {
        minimapCamera->Set(
            glm::vec3(train->position.x, 50, train->position.z),
            train->position,                                     // Center
            glm::vec3(0, 0, -1)                                  // Up
        );
    }
    DrawScene(minimapCamera->GetViewMatrix(), minimapProjection);
    glViewport(0, 0, resolution.x, resolution.y);
    if (isGameOver) {
        std::string message = "GAME OVER";
        float scale = 1.5f;
        glm::vec3 color = glm::vec3(1.0f, 0.0f, 0.0f);
        float textWidth = 0;
        for (char c : message) {
            gfxc::Character ch = textRenderer->Characters[c];
            textWidth += (ch.Advance >> 6) * scale;
        }

        float x = (resolution.x - textWidth) / 2.0f;
        float y = resolution.y / 2.0f;
        textRenderer->RenderText(message, x, y, scale, color);

        std::string scoreText = "Score: " + std::to_string(score);
        float scoreScale = 1.0f;
        float scoreWidth = 0;
        for (char c : scoreText) {
            gfxc::Character ch = textRenderer->Characters[c];
            scoreWidth += (ch.Advance >> 6) * scoreScale;
        }
        float xScore = (resolution.x - scoreWidth) / 2.0f;
        textRenderer->RenderText(scoreText, xScore, y + 60.0f, scoreScale, glm::vec3(1.0f, 1.0f, 1.0f));
    }
    else {
        // UI while playing
        textRenderer->RenderText("Score: " + std::to_string(score), 20, 30, 1.0f, glm::vec3(1));

        glm::vec3 timeColor = glm::vec3(1);
        if (gameTimeRemaining < 10) timeColor = glm::vec3(1, 0, 0);
        textRenderer->RenderText("Time: " + std::to_string((int)gameTimeRemaining), 20, 90, 1.0f, timeColor);
    }
}

void TrainGame::UpdateLogic(float deltaTimeSeconds) {
    train->Update(deltaTimeSeconds);
    CheckStationInteractions(deltaTimeSeconds);
    if (!isFreeCamera) {
        float cameraDist = 15.0f;
        float cameraHeight = 8.0f;
        float rotationOffset = 90.0f;

        float angleRad = glm::radians(train->angle + rotationOffset);

        glm::vec3 trainForward = glm::normalize(glm::vec3(sin(angleRad), 0, cos(angleRad)));
        glm::vec3 newCameraPos = train->position - (trainForward * cameraDist) + glm::vec3(0, cameraHeight, 0);

        glm::vec3 targetPoint = train->position + glm::vec3(0, 1.0f, 0);

        camera->Set(newCameraPos, targetPoint, glm::vec3(0, 1, 0));
    }
}

void TrainGame::FrameEnd()
{
}

void TrainGame::OnInputUpdate(float deltaTime, int mods)
{
    // move the camera only if MOUSE_RIGHT button is pressed
    if (window->MouseHold(GLFW_MOUSE_BUTTON_RIGHT))
    {
        float cameraSpeed = 50.0f;
        if (window->KeyHold(GLFW_KEY_W)) {
            camera->TranslateForward(cameraSpeed * deltaTime);
        }

        if (window->KeyHold(GLFW_KEY_A)) {
            camera->TranslateRight(-cameraSpeed * deltaTime);
        }

        if (window->KeyHold(GLFW_KEY_S)) {
            camera->TranslateForward(-cameraSpeed * deltaTime);

        }

        if (window->KeyHold(GLFW_KEY_D)) {
            camera->TranslateRight(cameraSpeed * deltaTime);
        }

        if (window->KeyHold(GLFW_KEY_Q)) {
            camera->TranslateUpward(-cameraSpeed * deltaTime);
        }

        if (window->KeyHold(GLFW_KEY_E)) {
            camera->TranslateUpward(cameraSpeed * deltaTime);
        }
    }
}

void TrainGame::RenderCommandQueue(const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix) {
    glm::vec3 startPos = glm::vec3(-5.0f, 5.0f, 0.0f);
    float spacing = 2.5f;

    for (int i = 0; i < currentOrder.size(); i++) {
        ResourceType res = currentOrder[i];

        glm::vec3 drawPos = startPos + glm::vec3(i * spacing, 0, 0);

        glm::mat4 mm = glm::translate(glm::mat4(1), drawPos);
        mm = glm::scale(mm, glm::vec3(1.5f, 1.5f, 1.5f));

        Mesh* meshToRender = nullptr;
        glm::vec3 centeringOffset = glm::vec3(0, 0, 0);

        if (res == WOOD) {
            meshToRender = meshes["station_wood"];
            centeringOffset = glm::vec3(-0.5f, -0.5f, -0.5f);
        }
        else if (res == OIL) {
            meshToRender = meshes["station_oil"];
            centeringOffset = glm::vec3(0.0f, -0.5f, 0.0f);
        }
        else if (res == STONE) {
            meshToRender = meshes["station_stone"];
            centeringOffset = glm::vec3(0.0f, 0.0f, 0.0f);
        }

        mm = glm::translate(mm, centeringOffset);
        if (meshToRender)
            RenderMeshCustom(meshToRender, shaders["VertexColor"], mm, viewMatrix, projectionMatrix);
    }
}

void TrainGame::DrawScene(const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix) {
    if (train)
        train->Render(meshes, shaders, viewMatrix, projectionMatrix);

    RenderCommandQueue(viewMatrix, projectionMatrix);

    for (auto rail : allRails) {
        if (!rail->isVisual) continue;

        float RAIL_WIDTH_NORMAL = 2.0f;
        float SLEEPER_THICKNESS = 0.5f;
        float RAIL_HEIGHT = 0.1f;

        glm::vec3 midPoint = (rail->startPosition + rail->endPosition) * 0.5f;

        glm::mat4 longElementMatrix = glm::mat4(1);
        longElementMatrix *= transform3D::Translate(midPoint.x, 0.05f, midPoint.z);
        longElementMatrix *= transform3D::RotateOY(rail->rotationAngleRads);

        // Ground/Water/Mountain
        {
            glm::mat4 groundM = glm::mat4(1);
            groundM *= transform3D::Translate(midPoint.x, 0, midPoint.z);
            groundM *= transform3D::RotateOY(rail->rotationAngleRads);
            groundM *= transform3D::Scale(8.0f, 0.1f, rail->length);
            groundM *= transform3D::Translate(-0.5f, 0, -0.5f);

            if (rail->type == RAIL_BRIDGE) {
                RenderMeshCustom(meshes["ground_water"], shaders["VertexColor"], groundM, viewMatrix, projectionMatrix);
            }
            else if (rail->type == RAIL_TUNNEL) {
                RenderMeshCustom(meshes["ground_mountain"], shaders["VertexColor"], groundM, viewMatrix, projectionMatrix);
            }
        }

        // Rails
        if (rail->type == RAIL_NORMAL) {
            int numSleepers = (int)(rail->length * 1.5f);
            glm::vec3 dir = rail->endPosition - rail->startPosition;
            for (int i = 0; i <= numSleepers; i++) {
                float t = (float)i / (float)numSleepers;
                glm::vec3 pos = rail->startPosition + dir * t;
                glm::mat4 sleeperM = glm::mat4(1);
                sleeperM *= transform3D::Translate(pos.x, 0.05f, pos.z);
                sleeperM *= transform3D::RotateOY(rail->rotationAngleRads);
                sleeperM *= transform3D::Scale(RAIL_WIDTH_NORMAL, RAIL_HEIGHT, SLEEPER_THICKNESS);
                sleeperM *= transform3D::Translate(-0.5f, 0.0f, -0.5f);
                RenderMeshCustom(meshes["sleeper"], shaders["VertexColor"], sleeperM, viewMatrix, projectionMatrix);
            }
            float metalRailWidth = 0.2f;
            float offsetMetal = 0.5f;
            glm::mat4 railL = longElementMatrix;
            railL *= transform3D::Translate(-offsetMetal, 0.15f, 0);
            railL *= transform3D::Scale(metalRailWidth, RAIL_HEIGHT, rail->length);
            railL *= transform3D::Translate(-0.5f, 0.0f, -0.5f);
            RenderMeshCustom(meshes["metal_rail"], shaders["VertexColor"], railL, viewMatrix, projectionMatrix);

            glm::mat4 railR = longElementMatrix;
            railR *= transform3D::Translate(offsetMetal, 0.15f, 0);
            railR *= transform3D::Scale(metalRailWidth, RAIL_HEIGHT, rail->length);
            railR *= transform3D::Translate(-0.5f, 0.0f, -0.5f);
            RenderMeshCustom(meshes["metal_rail"], shaders["VertexColor"], railR, viewMatrix, projectionMatrix);
        }
        else if (rail->type == RAIL_BRIDGE) {
            glm::mat4 midM = longElementMatrix;
            midM *= transform3D::Scale(RAIL_WIDTH_NORMAL, RAIL_HEIGHT, rail->length);
            midM *= transform3D::Translate(-0.5f, 0.0f, -0.5f);
            RenderMeshCustom(meshes["sleeper"], shaders["VertexColor"], midM, viewMatrix, projectionMatrix);

            float sideWidth = RAIL_WIDTH_NORMAL / 8.0f;
            float distantaX = (RAIL_WIDTH_NORMAL / 4.0f);

            glm::mat4 sideL = longElementMatrix;
            sideL *= transform3D::Translate(-distantaX, 0.1f, 0);
            sideL *= transform3D::Scale(sideWidth, RAIL_HEIGHT, rail->length);
            sideL *= transform3D::Translate(-0.5f, 0.0f, -0.5f);
            RenderMeshCustom(meshes["bridge_beam"], shaders["VertexColor"], sideL, viewMatrix, projectionMatrix);

            glm::mat4 sideR = longElementMatrix;
            sideR *= transform3D::Translate(distantaX, 0.1f, 0);
            sideR *= transform3D::Scale(sideWidth, RAIL_HEIGHT, rail->length);
            sideR *= transform3D::Translate(-0.5f, 0.0f, -0.5f);
            RenderMeshCustom(meshes["bridge_beam"], shaders["VertexColor"], sideR, viewMatrix, projectionMatrix);
        }
        else if (rail->type == RAIL_TUNNEL) {
            glm::mat4 thinLineM = longElementMatrix;
            thinLineM *= transform3D::Scale(0.5f, RAIL_HEIGHT, rail->length);
            thinLineM *= transform3D::Translate(-0.5f, 0.0f, -0.5f);
            RenderMeshCustom(meshes["sleeper"], shaders["VertexColor"], thinLineM, viewMatrix, projectionMatrix);

            int numSleepers = (int)(rail->length * 1.5f);
            glm::vec3 dir = rail->endPosition - rail->startPosition;
            for (int i = 0; i <= numSleepers; i++) {
                if (i % 5 == 0) {
                    float t = (float)i / (float)numSleepers;
                    glm::vec3 pos = rail->startPosition + dir * t;
                    glm::mat4 transM = glm::mat4(1);
                    transM *= transform3D::Translate(pos.x, 0.06f, pos.z);
                    transM *= transform3D::RotateOY(rail->rotationAngleRads);
                    transM *= transform3D::Scale(RAIL_WIDTH_NORMAL, RAIL_HEIGHT, SLEEPER_THICKNESS);
                    transM *= transform3D::Translate(-0.5f, 0.0f, -0.5f);
                    RenderMeshCustom(meshes["bridge_beam"], shaders["VertexColor"], transM, viewMatrix, projectionMatrix);
                }
            }
        }
    }

    // 4. Ground
    glm::mat4 modelMatrix = glm::mat4(1);
    modelMatrix = glm::translate(modelMatrix, glm::vec3(-125.0f, -0.1f, -125.0f));
    modelMatrix = glm::scale(modelMatrix, glm::vec3(250.0f, 0.1f, 250.0f));
    RenderMeshCustom(meshes["ground"], shaders["VertexColor"], modelMatrix, viewMatrix, projectionMatrix);

    // 5. Stations
    // Central Station (Color Changing)
    float timeFactor = gameTimeRemaining / TIME_LIMIT;
    int index = (int)(timeFactor * 9.0f);
    if (index < 0) index = 0;
    if (index > 9) index = 9;
    std::string meshName = "central_" + std::to_string(index);
    modelMatrix = glm::mat4(1);
    modelMatrix = glm::translate(modelMatrix, glm::vec3(-1.5, 0, -1.5));
    modelMatrix = glm::scale(modelMatrix, glm::vec3(3.0f, 2.0f, 3.0f));
    RenderMeshCustom(meshes[meshName], shaders["VertexColor"], modelMatrix, viewMatrix, projectionMatrix);

    // Wood Station (Check Cooldown)
    if (stations[1].cooldownTimer <= 0) {
        modelMatrix = glm::mat4(1);
        modelMatrix = glm::translate(modelMatrix, glm::vec3(-71, 0, -61));
        modelMatrix = glm::scale(modelMatrix, glm::vec3(2.0f, 1.5f, 2.0f));
        RenderMeshCustom(meshes["station_wood"], shaders["VertexColor"], modelMatrix, viewMatrix, projectionMatrix);
    }

    // Oil Station (Check Cooldown)
    if (stations[2].cooldownTimer <= 0) {
        modelMatrix = glm::mat4(1);
        modelMatrix = glm::translate(modelMatrix, glm::vec3(70, 0, -60));
        modelMatrix = glm::scale(modelMatrix, glm::vec3(1.5f, 3.0f, 1.5f));
        modelMatrix = glm::translate(modelMatrix, glm::vec3(0.0f, -0.5f, 0.0f));
        RenderMeshCustom(meshes["station_oil"], shaders["VertexColor"], modelMatrix, viewMatrix, projectionMatrix);
    }

    // Stone Station (Check Cooldown)
    if (stations[3].cooldownTimer <= 0) {
        modelMatrix = glm::mat4(1);
        modelMatrix = glm::translate(modelMatrix, glm::vec3(0, 3, 80));
        modelMatrix = glm::scale(modelMatrix, glm::vec3(2.0f, 2.0f, 2.0f));
        RenderMeshCustom(meshes["station_stone"], shaders["VertexColor"], modelMatrix, viewMatrix, projectionMatrix);
    }
}

void TrainGame::OnKeyPress(int key, int mods)
{
    if (!gameStarted) {
        gameStarted = true;
        switch (key) {
        case GLFW_KEY_1:
            train->SetSpeed(50);
            break;
        case GLFW_KEY_2:
            train->SetSpeed(40);
            break;
        case GLFW_KEY_3:
            train->SetSpeed(25);
            break;
        case GLFW_KEY_4:
            train->SetSpeed(20);
            break;
        case GLFW_KEY_5:
            train->SetSpeed(20);
            cooldownAmount = 20;
            break;
        default:
            gameStarted = false;
        }
    }
    if (key == GLFW_KEY_J)
        isFreeCamera = !isFreeCamera;
    if (key == GLFW_KEY_O)
        camera->SetProjectionMatrix(glm::ortho(left, right, bottom, top, zNear, zFar));
    if (key == GLFW_KEY_P)
        camera->SetProjectionMatrix(glm::perspective(fov, window->props.aspectRatio, 0.01f, 200.0f));
    // CENTER
    if (key == GLFW_KEY_W) train->ResolveIntersection(Train::DIR_FORWARD);

    // LEFT
    if (key == GLFW_KEY_Q) train->ResolveIntersection(Train::DIR_SOFT_LEFT);
    if (key == GLFW_KEY_A) train->ResolveIntersection(Train::DIR_HARD_LEFT);
    if (key == GLFW_KEY_Z) train->ResolveIntersection(Train::DIR_SHARP_LEFT);

    // RIGHT
    if (key == GLFW_KEY_E) train->ResolveIntersection(Train::DIR_SOFT_RIGHT);
    if (key == GLFW_KEY_D) train->ResolveIntersection(Train::DIR_HARD_RIGHT);
    if (key == GLFW_KEY_C) train->ResolveIntersection(Train::DIR_SHARP_RIGHT);
}


void TrainGame::OnKeyRelease(int key, int mods)
{
    // Add key release event
}


void TrainGame::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY)
{
    // Add mouse move event

    if (window->MouseHold(GLFW_MOUSE_BUTTON_RIGHT))
    {
        float sensivityOX = 0.001f;
        float sensivityOY = 0.001f;

        if (window->GetSpecialKeyState() == 0) {
            renderCameraTarget = false;
            camera->RotateFirstPerson_OY(-sensivityOY * deltaX);
            camera->RotateFirstPerson_OX(-sensivityOX * deltaY);
        }

        if (window->GetSpecialKeyState() & GLFW_MOD_CONTROL) {
            renderCameraTarget = true;
            camera->RotateThirdPerson_OY(-sensivityOY * deltaX);
            camera->RotateThirdPerson_OX(-sensivityOX * deltaY);
        }
    }
}

Rail* TrainGame::AddTrackSegment(glm::vec3 start, glm::vec3 end, float rotationIgnored, RailType type)
{
    glm::vec3 dir = end - start;

    // angle with oZ
    float angleRadians = atan2(dir.x, dir.z);
    float angleDegrees = glm::degrees(angleRadians);
    Rail* r_new = new Rail(start, end, angleDegrees, type);
    allRails.push_back(r_new);
    return r_new;
}

void TrainGame::CreateTrack() {
    for (auto rail : allRails) delete rail;
    allRails.clear();

    for (auto node : allIntersections) delete node;
    allIntersections.clear();

    Intersection* n_central = new Intersection(0, glm::vec3(0, 0, 0));
    Intersection* n_wood = new Intersection(1, glm::vec3(-70, 0, -60));
    Intersection* n_oil = new Intersection(2, glm::vec3(70, 0, -60));
    Intersection* n_stone = new Intersection(3, glm::vec3(0, 0, 80));

    Intersection* n_mid_left = new Intersection(4, glm::vec3(-40, 0, 30));
    Intersection* n_mid_center = new Intersection(5, glm::vec3(0, 0, 30));
    Intersection* n_mid_right = new Intersection(6, glm::vec3(40, 0, 30));

    Intersection* n_bot_left = new Intersection(7, glm::vec3(-40, 0, 60));
    Intersection* n_bot_mid = new Intersection(8, glm::vec3(0, 0, 60));
    Intersection* n_bot_right = new Intersection(9, glm::vec3(20, 0, 60));

    Intersection* n_top_mid_left = new Intersection(10, glm::vec3(-40, 0, -60));

    Intersection* n_sq_top_left = new Intersection(11, glm::vec3(0, 0, -100));
    Intersection* n_sq_mid_left = new Intersection(12, glm::vec3(0, 0, -60));
    Intersection* n_sq_bot_left = new Intersection(13, glm::vec3(0, 0, -20));

    Intersection* n_sq_top_right = new Intersection(14, glm::vec3(40, 0, -100));
    Intersection* n_sq_bot_right = new Intersection(15, glm::vec3(40, 0, -60));

    allIntersections = {
        n_central, n_wood, n_oil, n_stone,
        n_mid_left, n_mid_center, n_mid_right,
        n_bot_left, n_bot_mid, n_bot_right,
        n_top_mid_left, n_sq_top_left, n_sq_mid_left, n_sq_bot_left,
        n_sq_top_right, n_sq_bot_right
    };
    // START
    ConnectIntersections(n_central, n_mid_center, RAIL_NORMAL);

    // MIDDLE
    ConnectIntersections(n_mid_center, n_mid_left, RAIL_NORMAL);
    ConnectIntersections(n_mid_center, n_mid_right, RAIL_NORMAL);
    ConnectIntersections(n_mid_center, n_bot_mid, RAIL_NORMAL);

    // SOUTH
    ConnectIntersections(n_bot_mid, n_bot_left, RAIL_NORMAL);
    ConnectIntersections(n_bot_left, n_mid_left, RAIL_TUNNEL);

    // EAST
    ConnectIntersections(n_bot_mid, n_bot_right, RAIL_BRIDGE);
    ConnectIntersections(n_bot_right, n_mid_right, RAIL_BRIDGE);

    // STONE
    ConnectIntersections(n_bot_mid, n_stone, RAIL_BRIDGE);

    // TO NORTH
    ConnectIntersections(n_mid_left, n_top_mid_left, RAIL_TUNNEL);
    ConnectIntersections(n_mid_right, n_sq_bot_right, RAIL_BRIDGE);

    // NORTH

    ConnectIntersections(n_top_mid_left, n_wood, RAIL_TUNNEL);

    // TO SQUARE
    ConnectIntersections(n_top_mid_left, n_sq_mid_left, RAIL_NORMAL);

    // SQUARE LEFT
    ConnectIntersections(n_sq_mid_left, n_sq_top_left, RAIL_NORMAL);
    ConnectIntersections(n_sq_mid_left, n_sq_bot_left, RAIL_NORMAL);

    // SQUARE UP
    ConnectIntersections(n_sq_top_left, n_sq_top_right, RAIL_NORMAL);

    // SQUARE DIAG
    ConnectIntersections(n_sq_top_left, n_sq_bot_right, RAIL_NORMAL);

    // IN SQUARE
    ConnectIntersections(n_sq_top_right, n_sq_bot_right, RAIL_NORMAL);

    // OIL
    ConnectIntersections(n_sq_bot_right, n_oil, RAIL_NORMAL);

    // SQUARE DOWN
    ConnectIntersections(n_sq_bot_right, n_sq_bot_left, RAIL_NORMAL);

    // TO CENTER
    ConnectIntersections(n_sq_bot_left, n_central, RAIL_NORMAL);
}

void TrainGame::ConnectIntersections(Intersection* A, Intersection* B, RailType type) {
    // 1. A -> B
    glm::vec3 dirAB = B->position - A->position;
    float angleRadAB = atan2(dirAB.x, dirAB.z);
    float angleDegAB = glm::degrees(angleRadAB);

    Rail* railAB = new Rail(A->position, B->position, angleDegAB, type);
    railAB->destination = B;

    // 2. B -> A
    glm::vec3 dirBA = A->position - B->position;
    float angleRadBA = atan2(dirBA.x, dirBA.z);
    float angleDegBA = glm::degrees(angleRadBA);

    Rail* railBA = new Rail(B->position, A->position, angleDegBA, type);
    railBA->destination = A;

    railAB->twinRail = railBA;
    railBA->twinRail = railAB;

    railBA->isVisual = false;

    A->outgoingRails.push_back(railAB);
    B->outgoingRails.push_back(railBA);

    allRails.push_back(railAB);
    allRails.push_back(railBA);
}

void TrainGame::RenderMeshCustom(
    Mesh* mesh,
    Shader* shader,
    const glm::mat4& modelMatrix,
    const glm::mat4& viewMatrix,
    const glm::mat4& projectionMatrix)
{
    if (!mesh || !shader || !shader->program)
        return;
    shader->Use();
    glUniformMatrix4fv(shader->loc_view_matrix, 1, GL_FALSE, glm::value_ptr(viewMatrix));
    glUniformMatrix4fv(shader->loc_projection_matrix, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
    glUniformMatrix4fv(shader->loc_model_matrix, 1, GL_FALSE, glm::value_ptr(modelMatrix));

    mesh->Render();
}

void TrainGame::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods)
{
    // Add mouse button press event
}


void TrainGame::OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods)
{
    // Add mouse button release event
}


void TrainGame::OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY)
{
}


void TrainGame::OnWindowResize(int width, int height)
{
}
