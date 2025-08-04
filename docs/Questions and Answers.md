# Questions and Answers

## Q: Why is it called a "Scenario" instead of a "Level"?

Using the term **Scenario** emphasizes that each stage is a self-contained coding challenge with its own context or problem to solve. This fits the game's educational and narrative approach better than the generic term "Level".

- "Levels" suggest simple progression in difficulty.
- "Scenarios" represent real coding problems or missions, often with narrative elements.
- This term reinforces the idea that each stage is focused on learning or applying a specific programming concept (like Variables, Loops, or Functions).
- It aligns with how programming is typically practiced — by solving distinct problem scenarios, not just passing through levels.

---

## Q: Why is there no Admin Dashboard UI implemented in the current game?
- The game already handles all necessary data operations internally, and Supabase provides a full-featured database dashboard for administrators. A separate Admin Dashboard UI is considered optional and may be developed later using platforms like Vercel for more customized and user-friendly admin access. For now, it’s not essential to the gameplay or learning experience.

The current system uses Supabase’s built-in dashboard and the game’s own interface for all necessary data handling. A custom **Admin Dashboard UI** is not essential for gameplay or user experience right now.

- Supabase already provides a powerful web-based database dashboard for administrators.
- Game features like player progress, errors, and level data are managed internally.
- A separate Admin UI is considered **optional** and could be added later using platforms like **Vercel**, to create a user-friendly admin interface for managing players, player data or game settings.
