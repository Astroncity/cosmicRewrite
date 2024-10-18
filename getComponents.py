import os
import re
import click

# Regular expressions to find the patterns
component_decl_pattern = re.compile(r"ECS_COMPONENT_DECLARE\((\w+)\)")
typedef_pattern = re.compile(r"typedef\s+struct\s*{([^}]*)}\s*([\w\s,]+);")


@click.command()
@click.argument('directory', type=click.Path(exists=True))
@click.argument('output_file', type=click.File('w'))
def find_components(directory, output_file):
    """
    Recursively search through DIRECTORY for ECS_COMPONENT_DECLARE and typedef struct, 
    and dump component fields into OUTPUT_FILE, excluding 'flecs.h'.
    """
    components = {}

    # First, gather all ECS_COMPONENT_DECLARE components
    for root, _, files in os.walk(directory):
        for file in files:
            if file.endswith('.c') or file.endswith('.h'):  # Limit to C/C++ source files
                if file == 'flecs.h':  # Skip flecs.h
                    continue

                file_path = os.path.join(root, file)
                with open(file_path, 'r') as f:
                    content = f.read()

                    # Find all ECS_COMPONENT_DECLARE(<component>)
                    for match in component_decl_pattern.finditer(content):
                        component = match.group(1)
                        # Placeholder until typedef is found
                        components[component] = None

    # Now, search for typedef structs that define the components
    for root, _, files in os.walk(directory):
        for file in files:
            if file.endswith('.c') or file.endswith('.h'):
                if file == 'flecs.h':  # Skip flecs.h
                    continue

                file_path = os.path.join(root, file)
                with open(file_path, 'r') as f:
                    content = f.read()

                    # Find all typedef struct { ... } <component1>, <component2>, ...;
                    for match in typedef_pattern.finditer(content):
                        fields = match.group(1).strip()  # Struct fields
                        component_list = match.group(2).strip().split(
                            ',')  # List of components
                        component_list = [comp.strip()
                                          for comp in component_list]

                        # Check if any of the components in the list matches a declared component
                        for component in component_list:
                            if component in components:
                                components[component] = fields

    # Write results to the output file
    for component, fields in components.items():
        output_file.write(f"Component: {component}\n")
        if fields:
            # Normalize the field's indentation
            field_lines = fields.splitlines()
            normalized_fields = "\n".join(line.strip() for line in field_lines)
            output_file.write(f"Fields:\n{normalized_fields}\n")
        else:
            output_file.write("No typedef found.\n")
        output_file.write("\n")

    print(f"Component data written to {output_file.name}")


if __name__ == '__main__':
    find_components()
