const std = @import("std");

pub fn build(b: *std.Build) void {
    const target = b.standardTargetOptions(.{});
    const optimize = b.standardOptimizeOption(.{});

    const root_module = b.createModule(.{
        .target = target,
        .optimize = optimize,
        .link_libc = true, // TODO: no more libc
    });

    const c = b.addExecutable(.{
        .name = "noom",
        .root_module = root_module,
    });

    c.root_module.addCSourceFiles(.{
        .files = &[_][]const u8{
            "src/helper.c",
            "src/lexer.c",
            "src/parser.c",
            "src/main.c",
        }
    });

    b.installArtifact(c);

    const run_cmd = b.addRunArtifact(c);
    run_cmd.step.dependOn(b.getInstallStep());

    if (b.args) |args| {
        run_cmd.addArgs(args);
    }

    const run = b.step("run", "Build and run the project");
    run.dependOn(&run_cmd.step);
}
