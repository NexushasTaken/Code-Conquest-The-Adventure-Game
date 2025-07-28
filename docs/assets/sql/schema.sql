BEGIN;

-- Core Tables (Minimal Changes)
CREATE TABLE concepts (
  id UUID PRIMARY KEY,
  name TEXT NOT NULL UNIQUE,
  description TEXT
);

CREATE TABLE scenarios (
  id UUID PRIMARY KEY,
  concept_id UUID NOT NULL REFERENCES concepts(id),
  name TEXT NOT NULL,
  index INTEGER NOT NULL
);

-- Player Progress
CREATE TABLE player_scenario_runs (
  id UUID PRIMARY KEY,
  user_id UUID NOT NULL REFERENCES auth.users(id),
  scenario_id UUID NOT NULL REFERENCES scenarios(id),
  -- Scores
  speed_seconds FLOAT NOT NULL,
  accuracy_score INTEGER NOT NULL,
  efficiency_score FLOAT NOT NULL,
  -- Progress
  is_first_completion BOOLEAN NOT NULL DEFAULT FALSE,
  completion_count INTEGER NOT NULL DEFAULT 1,
  -- Timestamps
  completed_at TIMESTAMPTZ NOT NULL
);

CREATE TABLE player_concept_unlocks (
  user_id UUID REFERENCES auth.users(id),
  concept_id UUID REFERENCES concepts(id),
  is_unlocked BOOLEAN NOT NULL DEFAULT FALSE,
  unlocked_at TIMESTAMPTZ NOT NULL DEFAULT NOW(),
  PRIMARY KEY (user_id, concept_id)
);

-- Saves
CREATE TABLE player_scenario_saves (
  user_id UUID REFERENCES auth.users(id),
  scenario_id UUID REFERENCES scenarios(id),
  save_data JSONB NOT NULL,
  updated_at TIMESTAMPTZ NOT NULL DEFAULT NOW(),
  PRIMARY KEY (user_id, scenario_id)
);

-- Skin Collection
CREATE TABLE skins (
  id UUID PRIMARY KEY,
  identifier TEXT NOT NULL,
  name TEXT NOT NULL,
  cost NUMERIC,
  purchasable BOOLEAN NOT NULL DEFAULT FALSE,
  CONSTRAINT purchasable_cost_check CHECK (
    (purchasable = TRUE AND cost IS NOT NULL) OR
    (purchasable = FALSE AND cost IS NULL)
  )
);

-- Player Skins Collection
CREATE TABLE player_skins (
  user_id UUID REFERENCES auth.users(id),
  skin_id UUID REFERENCES skins(id),
  unlocked_at TIMESTAMPTZ NOT NULL DEFAULT NOW(),
  PRIMARY KEY (user_id, skin_id)
);

-- Player Selected Skins
CREATE TABLE player_selected_skin (
  user_id UUID REFERENCES auth.users(id),
  skin_id UUID REFERENCES skins(id),
  selected_at TIMESTAMPTZ NOT NULL DEFAULT NOW(),
  PRIMARY KEY (user_id),
  UNIQUE (user_id)
);

-- Achievements
CREATE TABLE achievements (
  id UUID PRIMARY KEY,
  name TEXT UNIQUE NOT NULL,
  description TEXT NOT NULL
);

-- Player Profile
CREATE TABLE player_profile (
  user_id UUID REFERENCES auth.users(id),
  username TEXT UNIQUE NOT NULL,
  coins NUMERIC DEFAULT 0,
  PRIMARY KEY (user_id, username)
);

-- Completed/Unlocked Achievements
CREATE TABLE player_achievements (
  user_id UUID REFERENCES auth.users(id),
  achievement_id UUID REFERENCES achievements(id),
  unlocked_at TIMESTAMPTZ NOT NULL DEFAULT NOW(),
  PRIMARY KEY (user_id, achievement_id)
);

-- Concept Proficiency
CREATE TABLE player_concept_proficiencies (
  user_id UUID REFERENCES auth.users(id),
  concept_id UUID REFERENCES concepts(id),
  proficiency FLOAT NOT NULL,
  last_updated TIMESTAMPTZ NOT NULL DEFAULT NOW(),
  PRIMARY KEY (user_id, concept_id)
);

-- Overall Mastery
CREATE TABLE player_masteries (
  user_id UUID REFERENCES auth.users(id),
  mastery FLOAT NOT NULL,
  last_updated TIMESTAMPTZ NOT NULL DEFAULT NOW(),
  PRIMARY KEY (user_id)
);

-- Leaderboards
CREATE TABLE scenario_best_scores (
  scenario_id UUID REFERENCES scenarios(id),
  speed_seconds FLOAT NOT NULL,
  accuracy_score INTEGER NOT NULL,
  efficiency_score FLOAT NOT NULL,
  updated_at TIMESTAMPTZ NOT NULL DEFAULT NOW(),
  PRIMARY KEY (scenario_id)
);

ROLLBACK;
