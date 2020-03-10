const Fs = require("fs");
const Path = require("path");

const version = Fs.readFileSync(Path.resolve("VERSION"), { encoding: "utf8" })
  .toString()
  .trim();

const updateVersion = file => {
  const packageJson = Fs.readFileSync(file, { encoding: "utf8" });
  const updatedPackageJson = JSON.parse(packageJson);
  updatedPackageJson.version = version;

  Fs.writeFileSync(file, JSON.stringify(updatedPackageJson, null, 2));
};

Fs.readdirSync(".", { encoding: "utf8" })
  .filter(file => file.includes(".json") && file.includes("morph"))
  .map(updateVersion);
