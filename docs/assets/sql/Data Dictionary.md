# Data Dictionary

| Name                                | Description                           | Attribute Name      | Attribute Type | Sample                                    | Location                            |
| ----------------------------------- | ------------------------------------- | ------------------- | -------------- | ----------------------------------------- | ----------------------------------- |
| Concept Primary Key                 | Unique identifier for concept         | id                  | UUID           | 123e4567-e89b-12d3-a456-426614174000      | public.concepts                     |
| Concept Name                        | Name of the programming concept       | name                | TEXT           | Variables & Data Types                    | public.concepts                     |
| Concept Description                 | Detailed explanation of concept       | description         | TEXT           | Basic variable declaration and data types | public.concepts                     |
| Scenario Primary Key                | Unique identifier for scenario        | id                  | UUID           | 123e4567-e89b-12d3-a456-426614174001      | public.scenarios                    |
| Scenario Concept Foreign Key        | Reference to associated concept       | concept_id          | UUID (FK)      | 123e4567-e89b-12d3-a456-426614174000      | public.scenarios                    |
| Scenario Name                       | Name of the scenario/level            | name                | TEXT           | Variable Basics                           | public.scenarios                    |
| Scenario Index                      | Order position of scenario            | index               | INTEGER        | 1                                         | public.scenarios                    |
| Run Primary Key                     | Unique identifier for player run      | id                  | UUID           | 123e4567-e89b-12d3-a456-426614174002      | public.player_scenario_runs         |
| Run User Foreign Key                | Reference to player                   | user_id             | UUID (FK)      | 123e4567-e89b-12d3-a456-426614174003      | public.player_scenario_runs         |
| Run Scenario Foreign Key            | Reference to scenario                 | scenario_id         | UUID (FK)      | 123e4567-e89b-12d3-a456-426614174001      | public.player_scenario_runs         |
| Run Speed                           | Completion time in seconds            | speed_seconds       | FLOAT          | 45.2                                      | public.player_scenario_runs         |
| Run Accuracy                        | Score based on correct lines          | accuracy_score      | INTEGER        | 95                                        | public.player_scenario_runs         |
| Run Efficiency                      | Score based on tiles used             | efficiency_score    | FLOAT          | 8.5                                       | public.player_scenario_runs         |
| Run First Completion                | First time completion flag            | is_first_completion | BOOLEAN        | TRUE                                      | public.player_scenario_runs         |
| Run Completion Count                | Times scenario completed              | completion_count    | INTEGER        | 3                                         | public.player_scenario_runs         |
| Run Completion Time                 | When scenario was completed           | completed_at        | TIMESTAMPTZ    | 2023-10-15 14:30:00+00                    | public.player_scenario_runs         |
| Unlock User Foreign Key             | Reference to player                   | user_id             | UUID (FK)      | 123e4567-e89b-12d3-a456-426614174003      | public.player_concept_unlocks       |
| Unlock Concept Foreign Key          | Reference to concept                  | concept_id          | UUID (FK)      | 123e4567-e89b-12d3-a456-426614174000      | public.player_concept_unlocks       |
| Unlock Status                       | Whether concept is unlocked           | is_unlocked         | BOOLEAN        | TRUE                                      | public.player_concept_unlocks       |
| Unlock Time                         | When concept was unlocked             | unlocked_at         | TIMESTAMPTZ    | 2023-10-15 14:25:00+00                    | public.player_concept_unlocks       |
| Save User Foreign Key               | Reference to player                   | user_id             | UUID (FK)      | 123e4567-e89b-12d3-a456-426614174003      | public.player_scenario_saves        |
| Save Scenario Foreign Key           | Reference to scenario                 | scenario_id         | UUID (FK)      | 123e4567-e89b-12d3-a456-426614174001      | public.player_scenario_saves        |
| Save Data                           | JSON save state data                  | save_data           | JSONB          | {"tiles": ["var", "x", "=", "5"]}         | public.player_scenario_saves        |
| Save Update Time                    | Last save update time                 | updated_at          | TIMESTAMPTZ    | 2023-10-15 14:35:00+00                    | public.player_scenario_saves        |
| Skin Primary Key                    | Unique identifier for skin            | id                  | UUID           | 123e4567-e89b-12d3-a456-426614174004      | public.skins                        |
| Skin Identifier                     | Unique skin identifier                | identifier          | TEXT           | "python_expert"                           | public.skins                        |
| Skin Name                           | Skin name                             | name                | TEXT           | "Python Expert"                           | public.skins                        |
| Skin Cost                           | Cost of the Skin in In-Game Currency  | cost                | NUMERIC        | 100                                       | public.skins                        |
| Skin is Purchasable                 | Indication if the Skin is purchasable | purchasable         | BOOLEAN        | TRUE                                      | public.skins                        |
| Player Skin User Foreign Key        | Reference to player                   | user_id             | UUID (FK)      | 123e4567-e89b-12d3-a456-426614174003      | public.player_skins                 |
| Player Skin Foreign Key             | Reference to skin                     | skin_id             | UUID (FK)      | 123e4567-e89b-12d3-a456-426614174004      | public.player_skins                 |
| Player Skin Unlock Time             | When skin was unlocked                | unlocked_at         | TIMESTAMPTZ    | 2023-10-15 14:40:00+00                    | public.player_skins                 |
| Selected Skin User Foreign Key      | Reference to player                   | user_id             | UUID (FK)      | 123e4567-e89b-12d3-a456-426614174003      | public.player_selected_skin         |
| Selected Skin Foreign Key           | Reference to selected skin            | skin_id             | UUID (FK)      | 123e4567-e89b-12d3-a456-426614174004      | public.player_selected_skin         |
| Selected Skin Time                  | When skin was selected                | selected_at         | TIMESTAMPTZ    | 2023-10-15 14:45:00+00                    | public.player_selected_skin         |
| Achievement Primary Key             | Unique identifier for achievement     | id                  | UUID           | 123e4567-e89b-12d3-a456-426614174005      | public.achievements                 |
| Achievement Name                    | Name of achievement                   | name                | TEXT           | First Program                             | public.achievements                 |
| Achievement Description             | Description of achievement            | description         | TEXT           | Completed your first program              | public.achievements                 |
| Player Profile User Foreign Key     | Reference to player                   | user_id             | UUID (FK)      | 123e4567-e89b-12d3-a456-426614174003      | public.player_profile               |
| Player Username                     | Player's unique display name          | username            | TEXT           | codeMaster99                              | public.player_profile               |
| Player Coins                        | Amount of in-game currency owned      | coins               | NUMERIC        | 250.00                                    | public.player_profile               |
| Player Achievement User Foreign Key | Reference to player                   | user_id             | UUID (FK)      | 123e4567-e89b-12d3-a456-426614174003      | public.player_achievements          |
| Player Achievement Foreign Key      | Reference to achievement              | achievement_id      | UUID (FK)      | 123e4567-e89b-12d3-a456-426614174005      | public.player_achievements          |
| Player Achievement Unlock Time      | When achievement was unlocked         | unlocked_at         | TIMESTAMPTZ    | 2023-10-15 14:50:00+00                    | public.player_achievements          |
| Proficiency User Foreign Key        | Reference to player                   | user_id             | UUID (FK)      | 123e4567-e89b-12d3-a456-426614174003      | public.player_concept_proficiencies |
| Proficiency Concept Foreign Key     | Reference to concept                  | concept_id          | UUID (FK)      | 123e4567-e89b-12d3-a456-426614174000      | public.player_concept_proficiencies |
| Proficiency Score                   | Player's proficiency score            | proficiency         | FLOAT          | 85.5                                      | public.player_concept_proficiencies |
| Proficiency Update Time             | Last proficiency update               | last_updated        | TIMESTAMPTZ    | 2023-10-15 14:55:00+00                    | public.player_concept_proficiencies |
| Mastery User Foreign Key            | Reference to player                   | user_id             | UUID (FK)      | 123e4567-e89b-12d3-a456-426614174003      | public.player_masteries             |
| Mastery Score                       | Player's overall mastery              | mastery             | FLOAT          | 78.3                                      | public.player_masteries             |
| Mastery Update Time                 | Last mastery update                   | last_updated        | TIMESTAMPTZ    | 2023-10-15 15:00:00+00                    | public.player_masteries             |
| Best Score Scenario Foreign Key     | Reference to scenario                 | scenario_id         | UUID (FK)      | 123e4567-e89b-12d3-a456-426614174001      | public.scenario_best_scores         |
| Best Speed Score                    | Fastest completion time               | speed_seconds       | FLOAT          | 30.5                                      | public.scenario_best_scores         |
| Best Accuracy Score                 | Highest accuracy achieved             | accuracy_score      | INTEGER        | 100                                       | public.scenario_best_scores         |
| Best Efficiency Score               | Most efficient solution               | efficiency_score    | FLOAT          | 10.0                                      | public.scenario_best_scores         |
| Best Score Update Time              | Last record update                    | updated_at          | TIMESTAMPTZ    | 2023-10-15 15:05:00+00                    | public.scenario_best_scores         |
