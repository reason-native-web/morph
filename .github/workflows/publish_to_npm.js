const cp = require("child_process");
const fs = require("fs");
const Path = require("path");

const getNameAndVersion = (package) => {
  const packageJson = fs.readFileSync(`${package}.json`, { encoding: "utf8" });
  const packageData = JSON.parse(packageJson);
  const version = fs
    .readFileSync(Path.resolve("VERSION"), { encoding: "utf8" })
    .toString()
    .trim();

  return { package, name: packageData.name, version };
};

const preparePackage = (packageData) => {
  const package = packageData.package;
  console.log(`Preparing package ${package}`);

  const packageJson = fs.readFileSync(`${package}.json`, { encoding: "utf8" });
  const updatedPackageJson = JSON.parse(packageJson);
  updatedPackageJson.version = packageData.version;

  fs.writeFileSync(
    Path.join(package, "package.json"),
    JSON.stringify(updatedPackageJson, null, 2)
  );
  fs.writeFileSync(
    Path.join(package, "dune-project"),
    `(lang dune 1.11)
`
  );

  fs.copyFileSync(`${package}.opam`, Path.join(package, `${package}.opam`));
  fs.copyFileSync(`LICENSE`, Path.join(package, "LICENSE"));
  fs.copyFileSync(`README.md`, Path.join(package, "README.md"));

  return package;
};

const deleteFolderRecursive = (path) => {
  if (fs.existsSync(path)) {
    fs.readdirSync(path).forEach((file, index) => {
      const curPath = Path.join(path, file);
      if (fs.lstatSync(curPath).isDirectory()) {
        // recurse
        deleteFolderRecursive(curPath);
      } else {
        // delete file
        fs.unlinkSync(curPath);
      }
    });
    fs.rmdirSync(path);
  }
};

const cleanPackage = (package) => {
  console.log(`Cleaning package ${package}`);

  fs.unlinkSync(Path.join(package, "package.json"));
  fs.unlinkSync(Path.join(package, `${package}.opam`));
  try {
    fs.unlinkSync(Path.join(package, `${package}.install`));
  } catch (e) {
    console.warn(`Could not remove ${package}.install in ${package}`);
  }
  fs.unlinkSync(Path.join(package, "LICENSE"));
  fs.unlinkSync(Path.join(package, "README.md"));
  try {
    fs.unlinkSync(Path.join(package, "dune-project"));
  } catch (e) {
    console.warn("Could not remove dune-file in ", package);
  }
  deleteFolderRecursive(Path.join(package, "_esy"));
  deleteFolderRecursive(Path.join(package, "esy.lock"));
  deleteFolderRecursive(Path.join(package, "node_modules"));
};

const buildAndPublishPackage = (package) => {
  console.log(`Building package ${package}`);
  const cwd = Path.resolve(package);
  try {
    cp.execSync("esy install", { cwd, encoding: "utf8" });
    cp.execSync("esy build", { cwd, encoding: "utf8" });
  } catch (e) {
    console.warn(e);
    return;
  }
  console.log(`Publishing package ${package}`);
  cp.execSync("npm publish --access public", {
    cwd,
    encoding: "utf8",
  });
};

const filterPackage = (packageInfo) => {
  try {
    const remoteVersion = cp
      .execSync(`npm view ${packageInfo.name} version`, { encoding: "utf8" })
      .toString()
      .trim();

    console.log({ ...packageInfo, remoteVersion });

    return remoteVersion !== packageInfo.version;
  } catch (e) {
    return true;
  }
};

const packages = fs
  .readdirSync(".", { encoding: "utf8" })
  .filter((file) => file.includes(".json") && file.includes("morph"))
  .map((file) => file.replace(".json", ""));

packages
  .map(getNameAndVersion)
  .filter(filterPackage)
  .map((packageData) => {
    preparePackage(packageData);
    return packageData;
  })
  .map((packageData) => {
    buildAndPublishPackage(packageData.package);
    return packageData;
  })
  .map((packageData) => {
    cleanPackage(packageData.package);
    return packageData;
  })
  .forEach((packageData) =>
    console.log(
      `Successfully published ${packageData.name}@${packageData.version}`
    )
  );
