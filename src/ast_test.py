import ast

code = """
"true" if True else "false"
"""

def print_ast(mod: ast.Module):
  print(ast.dump(mod))


try:
  # code = ast.unparse(tree)
  mod = ast.parse(code)  # Checks syntax
  print_ast(mod)
  print("✅ Valid Python")
except SyntaxError as e:
  print("❌ Invalid Python:", e)
